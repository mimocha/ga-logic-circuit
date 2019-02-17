/*	Simulation Bench

	Repo: https://github.com/mimocha/ga-logic-circuit
	Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns

module sim_bench;

	// Declare
	reg	clk;
	reg	rst;

// ========== Avalon Slave Port S1 ============ //

	reg	s1_read;
	reg	s1_write;
	reg	s1_address;
	wire	[31 : 0]	s1_readdata;
	reg	[31 : 0]	s1_writedata;

// ========== Avalon Slave Port S2 ============ //

	reg	s2_write;
	reg	[8 : 0]		s2_address;
	reg	[31 : 0]	s2_writedata;
	
// ========== Avalon Slave Port S3 ============ //

	reg	s3_write = 1'b0;
	reg	[15 : 0]	s3_writedata = 16'd100;
	
	initial
		begin : test_loop
		
		integer i;
		clk = 0;
		rst = 0;
		
		s1_read = 0;
		s1_write = 0;
		s1_address = 0;
		s1_writedata = 0;
		
		s2_write = 0;
		s2_address = 0;
		s2_writedata = 0;
		
		
		$display("   Time | S1 : WEN REN | ADDR | WRITE DATA || S2 : WEN | ADDR | WRITE DATA"); 
		
		$monitor(" %6d | S1 : %1b %1b | %1d | %8h || S2 : %1b | %3d | %8h",
			$time, s1_read, s1_write, s1_address, s1_writedata,
			s2_write, s2_address, s2_writedata);
		
		
		rst = 1;
		#10
		rst = 0;
		
		$display ("\n--- Set Linux In ---\n");
		
		#10
		s1_writedata = 32'hDEADBEEF;
		s1_address = 1;
		s1_write = 1;
		#10
		s1_write = 0;
		
		#10 
		s1_writedata = 32'hABCDEF12;
		s1_address = 0;
		s1_write = 1;
		#10
		s1_write = 0;
		
		// -- 0 --
		$display ("\n--- 0 ---\n");
			
		for (i = 511 ; i >= 0 ; i = i - 1) begin
			#1
			s2_address = i;
			s2_writedata = 32'h55555555;
			s2_write = 1;
			#1
			s2_write = 0;
		end
		
		$display ("\n--- S3 Begin ---\n");
		
		#10
		s3_write = 1;
		#10
		s3_write = 0;
		
		$display ("\n--- WAIT 5000 ---\n");
		#5000
		
		// -- END --
		$display ("\n--- END ---\n");
		
		#30 rst = 1;
		
		end
	
	// Clock
	always 
		begin
			#10 clk = !clk;
		end
	
	
	
	// --------------- Test Bench --------------- //
	
	cell_array u0 (
		.clk_in			( clk ),
		.rst			( rst ),
		.s1_read		( s1_read ),
		.s1_write		( s1_write ),
		.s1_address		( s1_address ),
		.s1_readdata	( s1_readdata ),
		.s1_writedata	( s1_writedata ),
		.s2_write		( s2_write ),
		.s2_address		( s2_address ),
		.s2_writedata	( s2_writedata ),
		.s3_write		( s3_write ),
		.s3_writedata	( s3_writedata )
	);
	
	
	
endmodule
