/* Cell Row Module
	This module simply generates a 1-D array of cells, of width DIMX.
*/

`timescale 1 ns / 1 ns

module cell_row 


//=========================
//  Parameters Definition
//=========================
#(
/* Cell Row X-dimension
	This should be explicitly redefined at every module instance.
	>	module #(.PARAMETER(VALUE)) module_instance ( ports );
*/
parameter DIMX = 64, // Array X-axis dimension
parameter PORT_WIDTH = 32,	// Port Width (bits)

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
input wire						clk,		// FPGA Clock Signal
input wire	[(DIMX) : 0]		linux_in,	// Linux -> Cell,		size (DIMX + 1)
input wire	[(DIMX) : 0]		cell_in,	// Neighbor -> Cell,	size (DIMX + 1)
input wire	[SLOTS-1 : 0]		write_en,	// Cell Write Enable,	size (DIMX * 4-bit / PORT WIDTH)
input wire	[(DIMX*4)-1 : 0]	ram,		// RAM -> Cell,			size (DIMX * 4-bit)
output		[(DIMX-1) : 0]		out			// Cell -> Output,		size (DIMX)
);


//=========================================================================
//  Structural Coding
//=========================================================================

genvar x;
generate
	for ( x = 0 ; x < DIMX ; x = x + 1 ) begin: gen_cells
		logic_block cell_inst (
			.clk			( clk ),
			.linux_in		( linux_in	[x +: 2] ),
			.cell_in		( cell_in	[x +: 2] ),		// Get the cell directly under, and the one next to it.
			.write_en		( write_en	[x/SLOTS] ),	// Assign every SLOTS number of cell to the same WREN wire
			.ram			( ram		[x*4 +: 4] ),
			.out			( out		[x] )
		);
	end
endgenerate

endmodule
