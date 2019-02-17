/* Logical Cell Row Module

    This module generates a 1-D array of cells, of size DIMX.
    Handles cell wire connections, looping, etc.
    
    Repo: https://github.com/mimocha/ga-logic-circuit
    Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns


module cell_row


//============================================================================
//  Parameters Definition
//============================================================================
#(
parameter DIMX = 64,        // Row Size (Logical Cell Count)
parameter PORT_WIDTH = 32,  // Avalon Slave Port Width (bits)
parameter SLOTS = (DIMX*4 / PORT_WIDTH) // Cell Write SLOTS -- See explanation in file 'cell_array.v'
)


//=======================================
//  Ports, Wires & Variables Definition
//=======================================
(
input wire                      clk,        // FPGA Clock
input wire                      rst,        // Reset
input wire  [DIMX-1 : 0]        in_cell,    // Cell Input,          size (DIMX)
input wire  [SLOTS-1 : 0]       we_ram,     // RAM Write Enable,    size (DIMX * 4-bit / PORT WIDTH)
input wire  [(DIMX*4)-1 : 0]    set_ram,    // RAM Set Data,        size (DIMX * 4-bit)
output      [DIMX-1 : 0]        out         // Cell Output,         size (DIMX)
);


//=========================================================================
//  Structural Coding
//=========================================================================

wire [(DIMX*2)-1 : 0] wire_to_cell;

// Generate Wire Connections For Top-Mid Rows
// Each cell is connected to two cell inputs
// Input Bit 0 (LSB) := Cell of the same index from the next row
// Input Bit 1 (MSB) := Cell of index+1 from the next row (Loops)

genvar i;
generate

    for (i = 0 ; i < DIMX ; i = i + 1) begin : gen_wire
        
        assign wire_to_cell [2*i]   = in_cell [i];
        assign wire_to_cell [2*i+1] = in_cell [(i+1) % DIMX];
        
    end
    
endgenerate


// Generate Cells
genvar x;
generate

    for ( x = 0 ; x < DIMX ; x = x + 1 ) begin: gen_cells
    
        logic_cell cell_inst (
            .clk            ( clk ),
            .rst            ( rst ),
            .in_signal      ( wire_to_cell  [x*2 +: 2] ),   // Assign 2 input signal wires to each cell
            .we_ram         ( we_ram    [x/SLOTS] ),        // Assign every SLOTS number of cell to the same WREN wire
            .set_ram        ( set_ram   [x*4 +: 4] ),       // Assign 4 RAM wires to each cell
            .out            ( out       [x] )               // Assign 1 input signal wire to each cell
        );
        
    end
    
endgenerate
    
endmodule
