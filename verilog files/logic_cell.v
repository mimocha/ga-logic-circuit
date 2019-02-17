/* Logical Cell Module

    A single logical cell.
    The cell can be configured to perform some logical function by setting its RAM.
    The cell performs the logical function with the input signals.
    The cell logic is updated on clock 
    
    Repo: https://github.com/mimocha/ga-logic-circuit
    Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns

module logic_cell (
    // Clock
    input   wire            clk,
    
    // Reset
    input   wire            rst,
    
    // Input Signal
    input   wire    [1:0]   in_signal,

    // RAM Write Enable
    input   wire            we_ram,

    // RAM Set Data -- 4-bits
    input   wire    [3:0]   set_ram,

    // Cell Output
    output  reg             out
);


/* Cell RAM
    Bit functions:
    MSB [    3 |    2 |     1 |     0 ] LSB
        [ NULL | NULL | RAM 1 | RAM 0 ]

    Truth Table:
        RAM 1 | RAM 0 |   Output
        ------+-------+----------
            0 |     0 |        0
            0 |     1 | Input [0]
            1 |     0 | Input [1]
            1 |     1 | Input [0] NAND Input [1]
*/
reg [3:0]   RAM;

// Asynchronous reset / write
always @ (posedge clk or posedge we_ram or posedge rst) begin

    if (rst == 1'b1) begin
        RAM <= 4'b0;
        out <= 0;
    end

    else if (we_ram == 1'b1) begin
        RAM <= set_ram;
    end

    else begin
        // Remember, the cases here are written in MSB format
        // Format: 4'b [RAM 3] [RAM 2] [RAM 1] [RAM 0] : *Do something*
        casex (RAM)
            4'b??00: out = 0;
            4'b??01: out = in_signal [0];
            4'b??10: out = in_signal [1];
            4'b??11: out = ~(in_signal[0] & in_signal[1]);
        endcase
    end
    
end


endmodule
