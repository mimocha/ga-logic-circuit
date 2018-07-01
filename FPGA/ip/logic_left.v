module logic_left (
	input	[2:0]	logic_in,
	output	[2:0]	logic_out
	);
	
	assign logic_out [0]	= (logic_in [0] | logic_in [1] | logic_in [2]);
	assign logic_out [2:1]	= 0;
endmodule 