/* Decoder Module
	
	Based on:
	http://www.asic-world.com/examples/verilog/decoder.html
	https://blog.digilentinc.com/multiplexers-decoders-encoders-and-shifters-in-verilog/
	https://stackoverflow.com/questions/50006983/101024-bit-decoder-in-verilog

	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns

module decoder 



//=========================================================================
//  Parameters Definition
//=========================================================================
#(
	parameter IN_WIDTH = 9,
	parameter OUT_WIDTH = (1 << IN_WIDTH)
)



//=========================================================================
//  Ports, Wires & Variables Definition
//=========================================================================
(
	input							enable,
	input		[IN_WIDTH-1 : 0]	binary_in,	//  binary input
	output wire	[OUT_WIDTH-1 : 0]	decoder_out	//  decoded output
);



//=========================================================================
//  Structural Coding
//=========================================================================
assign decoder_out = (enable) ? (1 << binary_in) : 0;


endmodule
