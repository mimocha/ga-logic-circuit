// Module definition for a single logic block

`timescale 1 ns / 1 ns

module logic_block (
	/* Cell Clock Signal */
	input	wire			clk,
	
	/* Linux Input Signal */
	input	wire	[1:0]	linux_in,
	
	/* Neighboring Cell Input Signal */
	input	wire	[1:0]	cell_in,
	
	/* Cell Write Enable */
	input	wire			write_en,
	
	/* Cell RAM Set -- Size (4-bits)
		Sets internal cell RAM to control cell function.
		
		Bit functions:
		MSB [     3 |    2 |     1 |     0 ] LSB
			[ STATE | NULL | RAM 1 | RAM 0 ]
		
		STATE Function:
			Sets cell to either Active (1) or Passive (0) state.
			Active state cells take input from Linux.
			Passive state cells take input from its neighbors.
		
		RAM Function:
			Sets what cell does with inputs:
				
			RAM 1 | RAM 0 |   Output
			------+-------+----------
			    0 |     0 |        0 
				0 |     1 | Input [0]
				1 |     0 | Input [1]
				1 |     1 | Input [0] NAND Input [1]
	*/
	input	wire	[3:0]	ram,
	
	/* Cell Output Signal */
	output	reg			out
);

/* 3-bit state register (RAM):
	bit 0 | RAM 0 | Left
	bit 1 | RAM 1 | Right
	bit 2 | NULL
	bit 3 | STATE
*/
reg	[3:0]	internal_state;

// Update state on write enable
always @ (posedge write_en) begin
	internal_state	<= ram;
end

// Update output logic on clock signal
always @ (posedge clk) begin
	
	// It is important to remember that integers are written in MSB format
	// ie, the first bit is the largest.
	casex (internal_state)
		// Null Output Mode
		4'b??00: out = 0;
		
		// CA Neighbor Input Mode
		4'b0?01: out = cell_in [0];
		4'b0?10: out = cell_in [1];
		4'b0?11: out = ~(cell_in[0] & cell_in[1]);
		
		// Linux Input Mode
		4'b1?01: out = linux_in [0];
		4'b1?10: out = linux_in [1];
		4'b1?11: out = ~(linux_in[0] & linux_in[1]);
		
		// Error Catch
		default: out = 1'bz;
	endcase

end

// Combinational Nodes Unavailable. Not enough combinational nodes on FPGA.
//assign out = 
//	(~internal_state [3] && ~internal_state [1] &&  internal_state [0] && cell_in [0])  ||
//	(~internal_state [3] &&  internal_state [1] && ~internal_state [0] && cell_in [0])  ||
//	(~internal_state [3] &&  internal_state [1] &&  internal_state [0] && ~(cell_in [0] && cell_in[1])) ||
//	
//	(internal_state [3] && ~internal_state [1] &&  internal_state [0] && linux_in [0])  ||
//	(internal_state [3] &&  internal_state [1] && ~internal_state [0] && linux_in [0])  ||
//	(internal_state [3] &&  internal_state [1] &&  internal_state [0] && ~(linux_in [0] && linux_in[1])) ;
	
endmodule 