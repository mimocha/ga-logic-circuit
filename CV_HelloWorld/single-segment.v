// Single 7 segment display 5161AS

// Module Definition
module single-segment (
	CLOCK,
	display-out
);

input CLOCK;
output [7:0] display-out;
wire CLOCK;
reg [7:0] display-out;

// Register
unsigned reg counter;

// Initial Block
initial begin
	counter = 0;
end

// Always Block
always @ (posedge CLOCK) begin

	counter = (counter + 1) % 10;
	
	display-out[0] <= 0;
	display-out[1] <= 0;
	display-out[2] <= 0;
	display-out[3] <= 0;
	display-out[4] <= 0;
	display-out[5] <= 0;
	display-out[6] <= 0;
	display-out[7] <= !display-out[7];
	
	case(counter)
		0:	display-out[0] <= 1;
			display-out[1] <= 1;
			display-out[2] <= 1;
			display-out[3] <= 1;
			display-out[4] <= 1;
			display-out[5] <= 1;
		
		1:	display-out[1] <= 1;
			display-out[2] <= 1;
		
		2:	display-out[0] <= 1;
			display-out[1] <= 1;
			display-out[3] <= 1;
			display-out[4] <= 1;
			display-out[6] <= 1;
			
		3:	display-out[0] <= 1;
			display-out[1] <= 1;
			display-out[2] <= 1;
			display-out[3] <= 1;
			display-out[6] <= 1;
		
		4:	display-out[1] <= 1;
			display-out[2] <= 1;
			display-out[5] <= 1;
			display-out[6] <= 1;
		
		5:	display-out[0] <= 1;
			display-out[2] <= 1;
			display-out[3] <= 1;
			display-out[5] <= 1;
			display-out[6] <= 1;
		
		6:	display-out[0] <= 1;
			display-out[2] <= 1;
			display-out[3] <= 1;
			display-out[4] <= 1;
			display-out[5] <= 1;
			display-out[6] <= 1;
		
		7:	display-out[0] <= 1;
			display-out[1] <= 1;
			display-out[2] <= 1;
		
		8:	display-out[0] <= 1;
			display-out[1] <= 1;
			display-out[2] <= 1;
			display-out[3] <= 1;
			display-out[4] <= 1;
			display-out[5] <= 1;
			display-out[6] <= 1;
		
		9:	display-out[0] <= 1;
			display-out[1] <= 1;
			display-out[2] <= 1;
			display-out[3] <= 1;
			display-out[5] <= 1;
			display-out[6] <= 1;
		
		default:	display-out[0] <= 1;
			display-out[3] <= 1;
			display-out[4] <= 1;
			display-out[5] <= 1;
			display-out[6] <= 1;
	endcase
	
end
	
	
endmodule 