/* Logical Cell Array

    A grid of size DIMX * DIMY of Logical Gates (Cells).
    Each cell can be configured to perform some logical function by setting its RAM.
    
    Repo: https://github.com/mimocha/ga-logic-circuit
    Copyright (c) 2018 Chawit Leosrisook
*/

`timescale 1 ns / 1 ns

module cell_array



//==================================================================================
//  Parameters Definition
//==================================================================================
#(
    // Cell Array Dimensions 
    parameter DIMX = 64,
    parameter DIMY = 64,

    /* Avalon Slave Port Width
        This specifies the width of output ports to/from Avalon Master devices.
        The port width should be set according to which AXI Interface is being used.
        
        Lightweight AXI supports 32 bit ports.
        Heavyweight AXI supports 32, 64, 128 bit ports.
    */
    parameter PORT_WIDTH = 32,

    /* Address Width Parameter (bits)
        This specifies the size of the address signal in bits.
        ADDRESS_WIDTH must be able to express the data required for each ports.
        
        > S1 - Linux IO
            Address range for the s1 read/write port is calculated by:
                (DIMX * 1 / PORT_WIDTH)
        
        
        > S2 - RAM
            Address range for the s2 read/write port is calculated by:
                (DIMX * DIMY * 4) / PORT_WIDTH
            
            The following is a quick calculation table for some possible combinations.
            
            DIMX x DIMY x 4 @ PW    : range (bits)
            ------------------------|---------------
            32 x 32 x 4 @  32 bit   : 128 ( 7 bit)
            32 x 32 x 4 @  64 bit   :  64 ( 6 bit)
            32 x 32 x 4 @ 128 bit   :  32 ( 5 bit)
            
            64 x 64 x 4 @  32 bit   : 512 ( 9 bit)
            64 x 64 x 4 @  64 bit   : 256 ( 8 bit)
            64 x 64 x 4 @ 128 bit   : 128 ( 7 bit)
            
            128 x 128 x 4 @  32 bit : 2048 ( 11 bit)
            128 x 128 x 4 @  64 bit : 1024 ( 10 bit)
            128 x 128 x 4 @ 128 bit :  512 (  9 bit)
        
    */
    parameter S1_ADDRESS_WIDTH = 1, // Linux IO
    parameter S2_ADDRESS_WIDTH = 9, // RAM
    
    /* Cell Write Enable "SLOT"

        As the Avalon Slave Port size is limited to 32, 64, or 128 bit width,
        it is simply not possible to set data to the entire Logical Cell Array in a single write operation.
        Thus, we divide the cell array and cell rows into "SLOTS" for writing data to.
        
        With each cell taking in 4-bits of RAM data, and each SLOT having size of PORT_WIDTH bits;
        the SLOT_SIZE is given as (PORT_WIDTH / 4). This is the number of cells a SLOT writes to.
        So PORT_WIDTH = 32 would create a SLOT_SIZE of 8; writing to 8 slots at a time.
        
        The number of SLOT each row has would be (DIMX / SLOT_SIZE).
        So a row with 64 cells would need 64/8 == 8 SLOTS.
        
        Simplifying this, the number of SLOTS a row has is (DIMX * 4 / PORT_WIDTH).
        The entire array would have a total of (DIMX * DIMY * 4 / PORT_WIDTH) SLOTS
        
        -----------------
        
        When writing RAM data to cells, 
        we fan out PORT_WIDTH bits of data to every SLOT_SIZE number of cells.
        
        The FPGA circuit then handles translating the ADDRESS to SLOT number.
        Only the addressed SLOT has Write_en = 1, and written to.
        All the other SLOTS have Write_en = 0, and ignores the data.
        
    */
    parameter SLOTS = (DIMX * 4 / PORT_WIDTH),
    
    /* Wind-Up Clock Counter Bit 
        
        How many bit the wind-up clock counter has.
        
    */
    parameter COUNTER_BIT = 16
)



//==================================================================================
//  Ports, Wires & Variables Definition
//==================================================================================
(
    // Clock
    input wire  clk_in,

    // Reset
    input wire  rst,

// ========== Avalon Slave Port S1 ============ //

    // S1 Read Enable
    input wire  s1_read,

    // S1 Write Enable
    input wire  s1_write,

    // S1 Read / Write Address
    input wire  [S1_ADDRESS_WIDTH-1 : 0]    s1_address,
    
    // S1 Read Data | Cell -> Linux
    output reg  [PORT_WIDTH-1 : 0]  s1_readdata,
    
    // S1 Write Data | Linux -> Cell
    input wire  [PORT_WIDTH-1 : 0]  s1_writedata,

// ========== Avalon Slave Port S2 ============ //

    // S2 Read Enable / Data
    // This Port is Write Only

    // S2 Write Enable
    input wire  s2_write,
    
    // S2 Write Address
    input wire  [S2_ADDRESS_WIDTH-1 : 0]    s2_address,

    // S2 Write Data
    input wire  [PORT_WIDTH-1 : 0]  s2_writedata,
    
// ========== Avalon Slave Port S3 ============ //

    // S2 Write Enable
    input wire  s3_write,

    // S2 Write Data
    input wire  [COUNTER_BIT-1 : 0] s3_writedata
    
    
);



//==================================================================================
//  Structural Coding
//==================================================================================


// ================================================================= //
// ======================= Wires & Registers ======================= //
// ================================================================= //

    /*  Internal Wires
        Connects each cell to its neighbors, and the input/output port.
        One wire per cell.
        Size (DIMX * DIMY)
    */
    wire    [(DIMX*DIMY)-1 : 0] internal_wire;

    /*  Linux Input Buffer Register -- s1_writedata
        Linux -> s1_writedata -> this register -> cell
        Created as a register, because the input must be constant.
        A wire that is larger than PORT_WIDTH could not be driven constantly.
        Inputs to the bottom most row of cells.
        Size (DIMX)
    */
    reg     [(DIMX)-1 : 0]  linux_in;

    /*  Linux Output Wire -- s1_readdata
        cell -> this wire -> s1_readdata -> Linux
        Connects the top most row output directly to the s1_readdata port.
        This is always driven by the cells, thus there is no need to create a buffer like the input port.
        Size (DIMX)
    */
    wire    [(DIMX)-1 : 0]  linux_out;
    assign  linux_out = internal_wire [(DIMX)-1:0];
    
    /*  RAM Wires
        Connects cell RAMs to s2_writedata port.
        s2_writedata is fanned out to this wire, to fill one cell row.
        This wire then fans out to the entire array.
        
        Fan out operation done using the replication operator {n{m}}
        Where:
            n := (DIMX * 4-bits) / PORT_WIDTH
            m := s2_writedata
            
        Essentially, copies the signal s2_writedata, SLOT number of times.
        (See more detail at explanation for parameter SLOT)
    */
    wire    [(DIMX*4)-1 : 0]    ram_wire;
    assign  ram_wire = { SLOTS {s2_writedata} };

    /*  Write Enable Wire
        Determines which SLOT accepts new RAM data, from s2_writedata.
        Driven constantly by a decoder.
        
        Size (DIMX * DIMY * 4-bits / PORT_WIDTH)
    */
    wire        [(DIMY*SLOTS)-1 : 0]    write_en;
    
    
    /* Controlled clock signal
        Uses the windup_clock module to control the clock given to the Cell Array.
        Can control precisely how many clock cycles to run.
    */
    wire    clk_out;
    
    
// =================================================================== //
// ======================= Generate Cell Array ======================= //
// =================================================================== //

    genvar y;
    generate
        for ( y = 0 ; y < DIMY ; y = y + 1 ) begin : gen_rows
        
        if (y < DIMY-1)
            begin   // Top & Middle rows
                cell_row        #(.DIMX(DIMX), .PORT_WIDTH(PORT_WIDTH))
                top_row (
                    .clk        ( clk_out ),
                    .rst        ( rst ),
                    .in_cell    ( internal_wire [(y+1)*DIMX +: DIMX] ), // Cell input comes from the next row
                    .we_ram     ( write_en [y*SLOTS +: SLOTS] ),
                    .set_ram    ( ram_wire ),
                    .out        ( internal_wire  [y*DIMX +: DIMX] )     // Cell output goes to wires of the same index
                );
            end
                
        else
            begin   // Bottom-most row, special module, loop from top & linux inputs
                cell_row_lin    #(.DIMX(DIMX), .PORT_WIDTH(PORT_WIDTH))
                bot_row (
                    .clk        ( clk_out ),
                    .rst        ( rst ),
                    .in_cell    ( internal_wire [0 +: DIMX] ),      // Cell input comes from the top-most row
                    .in_linux   ( linux_in ),                       // Connected to Linux Input Port
                    .we_ram     ( write_en [y*SLOTS +: SLOTS] ),
                    .set_ram    ( ram_wire ),
                    .out        ( internal_wire [y*DIMX +: DIMX] )  // Cell output goes to wires of the same index
                );
            end
            
        end
    endgenerate
    
    
// ================================================================== //
// ======================= Avalon Slave Ports ======================= //
// ================================================================== //

// More on the "+:" "-:" Vector bit-select operator:
// https://stackoverflow.com/questions/18067571/indexing-vectors-and-arrays-with

// -------------- S1 --------------

    always @ (posedge s1_read or posedge s1_write or posedge rst) begin
    
        // s1_readdata <= linux_out [s1_address]
        if (s1_read == 1'b1) begin
            s1_readdata <= linux_out [ (PORT_WIDTH * s1_address) +: PORT_WIDTH ];
        end
        
        // linux_in [s1_address] <= s1_writedata
        if (s1_write == 1'b1) begin
            linux_in [ (PORT_WIDTH * s1_address) +: PORT_WIDTH ] <= s1_writedata;
        end
        
        // Reset
        if (rst == 1'b1) begin
            linux_in <= 0;
        end
        
    end

    
// -------------- S2 --------------

    decoder     #(.IN(S2_ADDRESS_WIDTH))
    decoder_inst (
        .enable         ( s2_write ),
        .binary_in      ( s2_address ),     // 9-bit
        .decoder_out    ( write_en )        // 512-bit
    );
    
    
// -------------- S3 --------------
    
    windup_clock    #(.BIT(COUNTER_BIT))
    c0 (
        .clk_in     ( clk_in ),
        .rst        ( rst ),
        .wr_en      ( s3_write ),
        .wind       ( s3_writedata ),
        .clk_out    ( clk_out )
    );
    
    
endmodule
