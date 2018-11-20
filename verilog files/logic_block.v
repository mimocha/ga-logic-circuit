// Module definition for a single logic block

`timescale 1 ns / 1 ns

module logic_block (
	/* Cell Clock Signal */
	input	wire			clk,

	/* Input Signal */
	input	wire	[1:0]	in_signal,

	/* Cell Write Enable */
	input	wire			write_en,

	/* Cell RAM Set -- Size (4-bits)
		Sets internal cell RAM to control cell function.

		Bit functions:
		MSB [    3 |    2 |     1 |     0 ] LSB
			[ NULL | NULL | RAM 1 | RAM 0 ]

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
	bit 3 | NULL
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
		4'b??01: out = in_signal [0];
		4'b??10: out = in_signal [1];
		4'b??11: out = ~(in_signal[0] & in_signal[1]);

		// Error Catch
		default: out = 1'bz;
	endcase

end

endmodule
