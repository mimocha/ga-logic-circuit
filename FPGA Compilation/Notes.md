Notes on each version of the SOF (SRAM Object File) or JIC (JTAG Indirect Conversion) file

-----

# 2018-10-28 64x64

### Grid Size:	64 x 64

### Address Map:

* S1 - Linux IO	0x1000 - 0x1007
* S1 Port Width = 32 bit
* S1 Read Slot = 2

* S2 - RAM	0x0000 - 0x07FF
* S2 Port Width = 32 bit
* S2 Read Slot = 512

### Remarks:

 * First Working Prototype

-----

# 2018-10-29 00 64x64
## Basic 64x64 Non-looping Grid | 2 Input | 4-bit RAM | Cell states

### Grid Size:	64 x 64

### Address Map:

 * S1 - Linux IO	0x1000 - 0x1007
 * S1 Port Width = 32 bit
 * S1 Read Slot = 2

 * S2 - RAM	0x0000 - 0x07FF
 * S2 Port Width = 32 bit
 * S2 Read Slot = 512

### Remarks:

 * Avalon port read / writes now on clk signal instead of s1_read / s1_write / s2_read / s2_write

 * Each cell is connected to two other cells, a simple 1-dimensional Cellular Automaton.

 * Cells have 4-bit RAM:

	* LSB [RAM 0, RAM 1, EMPTY, STATE] MSB

	* RAMs bit 0 and 1 has the usual function of setting a cell's logic.

	* RAM bit 2 is unused, an empty buffer, so each cell takes in a nice, round 4-bits instead of 3-bits.

	* RAM bit 3 sets the STATE of each cell.
	A cell can be either ACTIVE (STATE == 1) or PASSIVE (STATE == 0). This sets where the cell takes its input from. Active cells get their inputs from the Linux IO, while Passive cells get their inputs from neighbor cells. This effectively creates an array with reconfigurable size. By setting an arbitrary row in the middle to ACTIVE state, it will perform the same function as the bottom row cells of the grid. The bottom row cells, meanwhile, will get their inputs from the Linux IO, no matter what state they are in.

 * No looping is present, this is a simple, rectangular, 2D plane.

 * Cells on the right edge of the grid, has one input wire disconnected (input bit 1, the MSB is disconnected).

 * Disconnected wires are connected to GND (zeroes).

 * Input / output port has the same width - 64-bits. However, as there is 128 input wires for each row of the grid, the input is set slightly different. Each cell takes the input of the same index as itself (ie, cell[0] takes input[0], cell[20] takes input[20]), and the input of index+1 (cell[0] also takes input[1], cell[20] also takes input[21], and so on). The last cell in the row, cell[63] will take inputs from input[63] and GND.



-----

# 2018-12-02
## 64x64 Torus Loop Grid | 2 Input | 4-bit RAM | No size reconfiguration | Windup-Clock

### Grid Size:	64 x 64

### Address Map:

* S1 - Linux IO	0x1000 - 0x1007
* S1 Port Width = 32 bit
* S1 Read Slot = 2

* S2 - RAM	0x2000 - 0x27FF
* S2 Port Width = 32 bit
* S2 Read Slot = 512

* S3 - Windup Clock 0x0000 - 0x0001

* Version ROM - 0x1_0000 - 0x1_000f

### Remarks:

 * Each cell is connected to two other cells.

 * Cells have 4-bit RAM:

	* LSB [RAM 0, RAM 1, EMPTY, EMPTY] MSB

	* RAMs bit 0 and 1 has the usual function of setting a cell's logic.

	* RAM bit 2 and 3 is unused, an empty buffer, this keeps the current format, and provides future expandability, up to 16 configs per cell.

 * Grid size is no longer reconfigurable. This function seems to not be needed, and is taking up unneccessary resources, so it will be removed.

 * Looping for X and Y axis.
 Inputs to cells on the right edge are now connected to the outputs of the cells on the left edge.
 Inputs for the bottom most row has also changed - they now connect to the outputs of the cells on the top row. (More on this)

 * Bottom most row inputs are now connected to both Linux IO and neighbor cells (the topmost row cells). The bottom row cells are each connected to inputs of the same index (ie, Cell[0] connects to input[0], and Cell[20] connects to input[20]), and is connected to the top row cells of index+1 (ie, Bottom Cell[0] is also connected to Top Cell[1], and Bottom Cell[63] is connected to Top Cell[0]).

 * Use Windup Clock to control how many clock pulse to give to the Cell Array. Precisely controls logic updating, and fixes unstable result state.

 * Version ROM implemented. Contains 16 character of version id.
