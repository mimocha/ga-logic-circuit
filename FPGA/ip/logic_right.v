module logic_right (
	input	[2:0]	logic_in,
	output	[2:0]	logic_out
	);
	
	assign logic_out [2]	= (logic_in [0] | logic_in [1] | logic_in [2]);
	assign logic_out [1:0]	= 0;
endmodule 