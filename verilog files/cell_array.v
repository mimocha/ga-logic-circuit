/* Definition for Cell Array Module

	All cells receive the same ram signals.
	WREN is used to determine which cells accept the new ram signal.

	Overflowed cell inputs are now looped around, instead of connecting to GND.
	Before :
		Cell[0][63] <- ( Cell[1][63], GND )
	After :
		Cell[0][63] <- ( Cell[1][63], Cell[1][0] )

	Bottom row cells take in Linux Inputs interspersed with Neighbor signals:
		[Linux 0 | Cell 1 | Linux 1 | Cell 2 | ... | Linux DIMX-1 | Cell 0]
	Each cell gets linux input of the same index (Cell[0] <- Linux[0], etc.)
	And gets Neighbor input from the top row, of index + 1 (Bottom Cell[0] <- Top Cell[1])

	The module contains 2 different Avalon Slave Ports.

	Port S1 -- Linux IO
		Could be set once and read after every settings,
		but it is best to reset this to zero, and set to desired values for every new individual.
		This would prevent any unknown / unexpected resets.

	Port S2 -- RAM
		Update for every SLOT. All SLOTs get the same RAM signal,
		WREN signal is used to determine which SLOT accepts the new change.

*/

`timescale 1 ns / 1 ns

module cell_array

//=========================
//  Parameters Definition
//=========================
#(
	/* Cell Array Dimensions
		The size of the cell array in the X and Y dimensions.
		Select powers of 2 for (relatively) simple address management.
	*/
	parameter DIMX = 64, // Array X-axis dimension
	parameter DIMY = 64, // Array Y-axis dimension

	/* Port Width Parameter (bits)
		This specifies the width of output ports to/from Avalon Master devices.
		The port width should be set according to which AXI Interface is being used.

		Lightweight AXI supports 32 bit ports.
		Heavyweight AXI supports 32, 64, 128 bit ports.
	*/
	parameter PORT_WIDTH = 32,

	/* Address Width Parameter (bits)
		This specifies the size of the address signal in bits.
		ADDRESS_WIDTH must be able to express the data required for each ports.

		> S1 - Linux IO
			Required range for the s1 read/write port is calculated by:
				(DIMX * 2) / PORT_WIDTH

			Although the s1_readdata port requires only half of s1_writedata,
			Avalon requires that both ports be of the same size.
			Thus s1_readdata is simply sending two copies of its data over:
				s1_readdata = {actual_data, actual_data}


		> S2 - RAM & State
			Required range for the s2 read/write port is calculated by:
				(DIMX * DIMY * 4) / PORT_WIDTH

			The following is a quick calculation table for some possible combinations.

			DIMX x DIMY x 4 @ PW		: decimal range (required bits)
			------------------------|---------------
			32 x 32 x 4 @  32 bit	: 128 ( 7 bit)
			32 x 32 x 4 @  64 bit	:  64 ( 6 bit)
			32 x 32 x 4 @ 128 bit	:  32 ( 5 bit)

			64 x 64 x 4 @  32 bit	: 512 ( 9 bit)
			64 x 64 x 4 @  64 bit	: 256 ( 8 bit)
			64 x 64 x 4 @ 128 bit	: 128 ( 7 bit)

			128 x 128 x 4 @  32 bit	: 2048 ( 11 bit)
			128 x 128 x 4 @  64 bit	: 1024 ( 10 bit)
			128 x 128 x 4 @ 128 bit	:  512 (  9 bit)

	*/
	parameter S1_ADDRESS_WIDTH = 1,	// Linux IO (bits)
	parameter S2_ADDRESS_WIDTH = 9,	// RAM (bits)

	/* Cell Write Enable "Slots"
		How many write actions are required for each row.
		Each row has (DIMX * 4-bit) bits of data,
		each write action has width (PORT_WIDTH).

		Thus, it requires (DIMX * 4 / PORT_WIDTH) writes to fill a row.
		ie, there are (DIMX * 4 / PORT_WIDTH) SLOTS of Cell Write Enable wires per row.
	*/
	parameter SLOTS = (DIMX * 4 / PORT_WIDTH)
)


//=======================================
//  Ports, Wires & Variables Definition
//=======================================
(
	/* Clock Signal -- clk */
	input wire	clk,

	/* Reset Signal -- reset */
	input wire	rst,

	// ========== Avalon Slave Port S1 ============ //

	/* Read Enable -- s1_read */
	input wire								s1_read,

	/* Write Enable -- s1_write */
	input wire								s1_write,

	/* Address -- s1_address */
	input wire	[S1_ADDRESS_WIDTH-1 : 0]	s1_address,

	/* Cell -> Linux -- s1_readdata */
	output reg	[PORT_WIDTH-1 : 0]			s1_readdata,

	/* Linux -> Cell -- s1_writedata */
	input wire	[PORT_WIDTH-1 : 0]			s1_writedata,


	// ========== Avalon Slave Port S2 ============ //

	/* Read Enable -- s2_read */
//	input wire								s2_read,

	/* Write Enable -- s2_write */
	input wire								s2_write,

	/* Address -- s2_address */
	input wire	[S2_ADDRESS_WIDTH-1 : 0]	s2_address,

	/* Cell RAM Read -- s2_readdata */
//	output reg	[PORT_WIDTH-1 : 0]			s2_readdata,

	/* Cell RAM Write -- s2_writedata */
	input wire	[PORT_WIDTH-1 : 0]			s2_writedata

);

//=========================================================================
//  Structural Coding
//=========================================================================

// ===== Wires & Registers ===== //

	/* Internal Cell Wires
		Connects each cell to its neighbors.

		Cell [0][0] ---> Cell [1][0]
		Cell [0][1] -^
		Cell [0][1] ---> Cell [1][1]
		Cell [0][2] -^

		Size (DIMX * DIMY)
	*/
	wire	[(DIMX*DIMY)-1 : 0]	internal_wire;

	/* Linux -> Cell Data -- s1_writedata
		Inputs to cell array as given by the Linux Kernel.
		Inputs to a single row of cells.
		Size (DIMX)
	*/
	reg		[(DIMX)-1 : 0]	linux_in;

	/* Cell -> Linux Data -- s1_readdata
		Outputs from cell array to Linux Kernel.
		Size (DIMX)
	*/
	wire	[(DIMX)-1 : 0]	linux_out;
	assign	linux_out = internal_wire [(DIMX)-1:0];

	/* RAM Wires
		Linux writes to wire "s2_writedata" of width (PORT_WIDTH)
		"s2_writedata" then fans out to wire "wire_to_ram", using the replication operator {n{m}}
		The wire "wire_to_ram" has a width (DIMX * 4), to span 1 row of cells.
		Every row gets the same copy of "wire_to_ram".
		The Write Enable signal is used to determine which cells would be set.

		When fanning out wire "s2_writedata" with operator {n{m}},
		the number of replications n is calculated by:
			(DIMX * 4-bits) / PORT_WIDTH
	*/
	wire	[(DIMX*4)-1 : 0]	wire_to_ram;
	assign 	wire_to_ram = {(DIMX*4/PORT_WIDTH){s2_writedata}};

	/* Write Enable Register
		Connects (DIMX * 4-bits / PORT_WIDTH) number of cells to one WREN signal.
		Size (DIMX * DIMY * 4-bits / PORT_WIDTH)
	*/
	reg		[(DIMX*DIMY*4/PORT_WIDTH)-1 : 0]		write_en = 0; // 512-bits

// ===== Generate Cell Array ===== //

	genvar y;
	generate
		for ( y = 0 ; y < DIMY ; y = y + 1 ) begin : gen_rows

		if (y < DIMY-1)
				begin	// Top rows, all similar
				cell_row	#(.DIMX(DIMX), .PORT_WIDTH(PORT_WIDTH))	// Parameter Redefinition
				top_row (
					.clk			( clk ),

					// TODO: document this:
					// internal_wire, Take the next row's output, append 0 to the end (MSB)
					.cell_in		( {1'b0, internal_wire  [((y+1)*DIMX) +: DIMX]} ),

					.write_en		( write_en [y*SLOTS +: SLOTS] ),	// Size (DIMX * 4 / PORT_WIDTH)
					.ram			( wire_to_ram ),
					.out			( internal_wire  [y*DIMX +: DIMX] )
				);
				end

			else
				begin	// Bottom-most row, no neighbors
				cell_row	#(.DIMX(DIMX), .PORT_WIDTH(PORT_WIDTH))	// Parameter Redefinition
				bot_row (
					.clk			( clk ),

					// TODO: document this:
					// linux_in, append 0 to the end (MSB)
					.cell_in		( {1'b0, linux_in} ),

					.write_en		( write_en [y*SLOTS +: SLOTS] ),	// Size (DIMX * 4 / PORT_WIDTH)
					.ram			( wire_to_ram ),
					.out			( internal_wire  [y*DIMX +: DIMX] )
				);
				end

		end
	endgenerate

// ===== Avalon Slave Ports ===== //
// More on the "+:" "-:" Vector bit-select operator:
// https://stackoverflow.com/questions/18067571/indexing-vectors-and-arrays-with

/* Avalon Slave Port S1 */

	// s1_readdata <= linux_out [s1_address]
	always @ (posedge clk) begin
		if (s1_read == 1) begin
			s1_readdata = linux_out [ (PORT_WIDTH * s1_address) +: PORT_WIDTH ];
		end
	end

	// linux_in [s1_address] <= s1_writedata
	always @ (posedge clk) begin
		if (s1_write == 1) begin
			linux_in [ (PORT_WIDTH * s1_address) +: PORT_WIDTH ] = s1_writedata;
		end
	end

/* Avalon Slave Port S2 */

	// s2_writedata write enable
	always @ (posedge clk) begin
		if (s2_write == 1) begin
			write_en [s2_address] = 1'b1;
		end
		else begin
			write_en [s2_address] = 1'b0;
		end
	end

endmodule
