`define enable_ADC
`define enable_ARDUINO
`define enable_GPIO0
`define enable_GPIO1
//`define enable_HPS

module fpga_main (
	//////////// CLOCK //////////
	input						FPGA_CLK1_50,
	input						FPGA_CLK2_50,
	input						FPGA_CLK3_50,

`ifdef enable_ADC
	//////////// ADC //////////
	/* 3.3-V LVTTL */
	output						ADC_CONVST,
	output						ADC_SCLK,
	output						ADC_SDI,
	input						ADC_SDO,
`endif

`ifdef enable_ARDUINO
	//////////// ARDUINO ////////////
	/* 3.3-V LVTTL */
	inout					[15:0]	ARDUINO_IO,
	inout							ARDUINO_RESET_N,
`endif

`ifdef enable_GPIO0
	//////////// GPIO 0 ////////////
	/* 3.3-V LVTTL */
	inout				[35:0]		GPIO_0,
`endif

`ifdef enable_GPIO1
	//////////// GPIO 1 ////////////
	/* 3.3-V LVTTL */
	inout				[35:0]		GPIO_1,
`endif

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

	//////////// KEY ////////////
	/* 3.3-V LVTTL */
	input				[1:0]			KEY,

	//////////// LED ////////////
	/* 3.3-V LVTTL */
	output				[7:0]			LED,

	//////////// SW ////////////
	/* 3.3-V LVTTL */
	input				[3:0]			SW

);

// Simple Test
//	bit			[3:0][3:0]	ARRAY;
//
//	assign ARRAY[0][0] = SW[0];
//	assign ARRAY[1][0] = SW[1];
//	assign ARRAY[2][0] = SW[2];
//	assign ARRAY[3][0] = SW[3];
//	
//	assign ARRAY[0][3:1] = ARRAY[0][2:0];
//	assign ARRAY[1][3:1] = ARRAY[1][2:0];
//	assign ARRAY[2][3:1] = ARRAY[2][2:0];
//	assign ARRAY[3][3:1] = ARRAY[3][2:0];
//	
//	assign LED[0] = ARRAY[0][3];
//	assign LED[1] = ARRAY[1][3];
//	assign LED[2] = ARRAY[2][3];
//	assign LED[3] = ARRAY[3][3];
//	
//	assign GPIO_0[0] = ARRAY[0][3];
//	assign GPIO_0[1] = ARRAY[1][3];


// Module Instantiation Test
	wire	[7:0]	address_sig;// Address to write to
	wire	[7:0]	data_sig;	// Data to write
	wire			wren_sig;	// Write Enable Signal
	wire	[7:0]	q_sig;		// Output data from current address
	wire			ram_busy;	// Ram is busy signal
	wire			init_sig;	// Initialize Signal
	
// 256 8-bit Words == 256 Byte Data
ram_1port	ram_1port_0 (
	.address	( address_sig ),
	.clock		( FPGA_CLK1_50 ),
	.data		( data_sig ),
	.wren		( wren_sig ), // Write on 0
	.q			( q_sig )
	);

ram_init	ram_init_0 (
	.clock			( FPGA_CLK1_50 ),
	.init			( init_sig ),
	.dataout		( dataout_sig ),
	.init_busy		( ram_busy ),
	.ram_address	( address_sig ),
	.ram_wren		( wren_sig )
	);

	
// RAM Usage Test
	reg		[]	counter


// Update (Display data at current pointer)
always @ ( posedge FPGA_CLK1_50 ) begin
	

	// Read only if RAM is not busy
	if ( ram_busy != 1'b1 ) begin
		LED [7:0] <= q_sig [7:0];
	end
end



// Move Address Pointer
always @ ( posedge KEY [0] ) begin
	init_sig <= 1'b1;
endmodule

