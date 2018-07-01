
`define enable_HPS

module fpga_main (
	input		FPGA_CLK1_50,
	input		FPGA_CLK2_50,
	input		FPGA_CLK3_50,

	output		ADC_CONVST,
	output		ADC_SCLK,
	output		ADC_SDI,
	input		ADC_SDO,

	inout		[15:0]	ARDUINO_IO,
	inout				ARDUINO_RESET_N,

	inout		[35:0]	GPIO_0,
	inout		[35:0]	GPIO_1,
	
	`ifdef enable_HPS
	//////////// HPS //////////
	/* 3.3-V LVTTL */
	inout						HPS_CONV_USB_N,

	/* SSTL-15 Class I */
	output			[14:0]		HPS_DDR3_ADDR,
	output			 [2:0]		HPS_DDR3_BA,
	output						HPS_DDR3_CAS_N,
	output						HPS_DDR3_CKE,
	output						HPS_DDR3_CS_N,
	output			 [3:0]		HPS_DDR3_DM,
	inout			[31:0]		HPS_DDR3_DQ,
	output						HPS_DDR3_ODT,
	output						HPS_DDR3_RAS_N,
	output						HPS_DDR3_RESET_N,
	input						HPS_DDR3_RZQ,
	output						HPS_DDR3_WE_N,
	/* DIFFERENTIAL 1.5-V SSTL CLASS I */
	output						HPS_DDR3_CK_N,
	output						HPS_DDR3_CK_P,
	inout			 [3:0]		HPS_DDR3_DQS_N,
	inout			 [3:0]		HPS_DDR3_DQS_P,
	
	/* 3.3-V LVTTL */
	output						HPS_ENET_GTX_CLK,
	inout						HPS_ENET_INT_N,
	output						HPS_ENET_MDC,
	inout						HPS_ENET_MDIO,
	input						HPS_ENET_RX_CLK,
	input			 [3:0]		HPS_ENET_RX_DATA,
	input						HPS_ENET_RX_DV,
	output			 [3:0]		HPS_ENET_TX_DATA,
	output						HPS_ENET_TX_EN,
	inout						HPS_GSENSOR_INT,
	inout						HPS_I2C0_SCLK,
	inout						HPS_I2C0_SDAT,
	inout						HPS_I2C1_SCLK,
	inout						HPS_I2C1_SDAT,
	inout						HPS_KEY,
	inout						HPS_LED,
	inout						HPS_LTC_GPIO,
	output						HPS_SD_CLK,
	inout						HPS_SD_CMD,
	inout			 [3:0]		HPS_SD_DATA,
	output						HPS_SPIM_CLK,
	input						HPS_SPIM_MISO,
	output						HPS_SPIM_MOSI,
	inout						HPS_SPIM_SS,
	input						HPS_UART_RX,
	output						HPS_UART_TX,
	input						HPS_USB_CLKOUT,
	inout			 [7:0]		HPS_USB_DATA,
	input						HPS_USB_DIR,
	input						HPS_USB_NXT,
	output						HPS_USB_STP,
	`endif
	
	input		[1:0]	KEY,
	output		[7:0]	LED,
	input		[3:0]	SW
	
	);


// ----- Module Instantiation Test -----


//	wire	[7:0]	address_sig;// Address to write to
//	wire	[7:0]	data_sig;	// Data to write
//	wire			wren_sig;	// Write Enable Signal
//	wire	[7:0]	q_sig;		// Output data from current address
//	
//// 256 8-bit Words == 256 Byte Data == 256 circuit blocks
//ram_1port	ram_1port_0 (
//	.address	( address_sig ),
//	.clock		( FPGA_CLK1_50 ),
//	.data		( data_sig ),
//	.wren		( wren_sig ), // Write on 0
//	.q			( q_sig )
//	);

//	wire	[2:0]	logic_in_sig;
//	wire	[2:0]	logic_out_sig;
	
// Logic Grid Module
//logic_grid	logic_grid_0 (
//	.clock		( FPGA_CLK1_50 ),
//	.wren		( wren_sig ),
//	.login		( logic_in_sig ),
//	.logout		( logic_out_sig )
//	);
//	
//// Circuit Block System	
//circuit_block	circuit_block_0 (
//	.clk_clk			( FPGA_CLK1_50 ), // Clock
//	.memory_mem_a		(  ),	// Memory Address
//	.memory_mem_ba		(  ),	// Memory Address
//	.memory_mem_ck		(  ),	// Memory Clock Pos
//	.memory_mem_ck_n	(  ),	// Memory Clock Neg
//	.memory_mem_cke		(  ),	// Memory Clock Enable
//	.memory_mem_cs_n	(  ),	//
//	.memory_mem_ras_n	(  ),	//
//	.memory_mem_cas_n	(  ),	//
//	.memory_mem_we_n	(  ),	// Memory Write Enable
//	.memory_mem_reset_n	(  ),	// Memory Reset
//	.memory_mem_dq		(  ),	//
//	.memory_mem_dqs		(  ),	// Memory Bus Pos
//	.memory_mem_dqs_n	(  ),	// Memory Bus Neg
//	.memory_mem_odt		(  ),	//
//	.memory_mem_dm		(  ),	//
//	.memory_oct_rzqin	(  ),	//
//);

endmodule
