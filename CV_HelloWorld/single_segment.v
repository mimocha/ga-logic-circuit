// Single 7 segment display 5161AS

// Module Definition
module single_segment (
	CLOCK,
	display_out
);

input 			CLOCK;
output 	[7:0] 	display_out;
wire 			CLOCK;
reg 	[7:0] 	display_out;
reg 	[31:0]	counter; // 32 bit -> 22 bit == 1 sec @ 5 mhz clk
reg		[3:0]	digit;
reg		[3:0]	i;

// Initial Block
initial begin
	counter = 32'h0;
	digit = 4'h0;
	i = 3'h0;
end

// Always Block
always @ (posedge CLOCK) 
begin
	counter = counter + 32'h1;
	i = 3'h0;
	
	if (counter % 32'h200000 == 0) begin
		
		// Display Counter
		digit = (digit + 4'h1) % 4'hA;
		
		// Clear Display
		repeat (7) 
			begin
				display_out[i] = 1'b0;
				i = i + 3'h1;
			end
		
		// Blink Decimal
		display_out[7] = !display_out[7];
		
		// Set Display
		case(digit)
			4'h0:	begin
					display_out[0] <= 1'b1;
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[4] <= 1'b1;
					display_out[5] <= 1'b1;
					end
			
			4'h1:	begin
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					end
			
			4'h2:	begin
					display_out[0] <= 1'b1;
					display_out[1] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[4] <= 1'b1;
					display_out[6] <= 1'b1;
					end
				
			4'h3:	begin
					display_out[0] <= 1'b1;
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[6] <= 1'b1;
					end
					
			4'h4:	begin
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[5] <= 1'b1;
					display_out[6] <= 1'b1;
					end
					
			4'h5:	begin
					display_out[0] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[5] <= 1'b1;
					display_out[6] <= 1'b1;
					end
					
			4'h6:	begin
					display_out[0] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[4] <= 1'b1;
					display_out[5] <= 1'b1;
					display_out[6] <= 1'b1;
					end
					
			4'h7:	begin
					display_out[0] <= 1'b1;
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					end
					
			4'h8:	begin
					display_out[0] <= 1'b1;
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[4] <= 1'b1;
					display_out[5] <= 1'b1;
					display_out[6] <= 1'b1;
					end
					
			4'h9:	begin
					display_out[0] <= 1'b1;
					display_out[1] <= 1'b1;
					display_out[2] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[5] <= 1'b1;
					display_out[6] <= 1'b1;
					end
					
			default:begin
					display_out[0] <= 1'b1;
					display_out[3] <= 1'b1;
					display_out[4] <= 1'b1;
					display_out[5] <= 1'b1;
					display_out[6] <= 1'b1;
					end
		endcase
		
	end
	
end
	
	
endmodule 