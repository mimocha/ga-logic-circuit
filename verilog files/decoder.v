/* Verilog HDL Decoder Module

	Simple N-to-One-hot Binary Decoder.

	Based on:
	http://www.asic-world.com/examples/verilog/decoder.html
	https://blog.digilentinc.com/multiplexers-decoders-encoders-and-shifters-in-verilog/
	https://stackoverflow.com/questions/50006983/101024-bit-decoder-in-verilog

	Repo: https://github.com/mimocha/verilog-library
	Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns

module decoder

//=========================================================================
//  Parameters Definition
//=========================================================================
#(
	// Input bit count
	parameter IN = 9,

	// Output bit count
	parameter OUT = (1 << IN)
)

//=========================================================================
//  Ports, Wires & Variables Definition
//=========================================================================
(
	input					enable,
	input		[IN-1 : 0]	binary_in,	//  binary input
	output wire	[OUT-1 : 0]	decoder_out	//  decoded output
);

//=========================================================================
//  Structural Coding
//=========================================================================

assign decoder_out = (enable) ? (1 << binary_in) : 0;


endmodule
