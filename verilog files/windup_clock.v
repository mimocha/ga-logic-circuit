/* Verilog HDL Wind-Up Clock Module

    Filters clock signal, outputs the specified number of clock pulses.
    Analogous to a wind-up clock,

    Repo: https://github.com/mimocha/verilog-library
    Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns

module windup_clock

//=========================================================================
//  Parameters Definition
//=========================================================================
#(
    // Internal counter bit count
    parameter BIT = 16
)

//=========================================================================
//  Ports, Wires & Variables Definition
//=========================================================================
(
    input wire          clk_in,
    input wire          rst,
    input wire          wr_en,
    input wire  [BIT-1 : 0] wind,
    output wire         clk_out
);

//=========================================================================
//  Structural Coding
//=========================================================================

reg [BIT-1 : 0] counter;

// While any counter bit is high && wr_en is low, let CLK signal through.
assign clk_out = clk_in & |counter & ~wr_en;


always @ (posedge clk_in or posedge wr_en or posedge rst) begin

    // Reset
    if (rst == 1'b1) begin : reset
        counter <= 0;
    end

    // Wind Up Counter
    else if (wr_en == 1'b1) begin : winding
        counter <= wind;
    end
    
    // Countdown to 0, and stops
    else if (counter > 1'b0) begin : countdown
        counter = counter - 1'b1;
    end
end


endmodule
