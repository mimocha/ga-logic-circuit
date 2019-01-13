# Generation of Logic Circuits with Genetics Algorithm
-----

## Background:

 The main objective for this dissertation is to research the application of evolutionary hardware, and implementing genetics algorithm and cellular automaton to generate logical circuits.

 The research is done on the Terasic DE0-Nano-SoC development board. The board is equipped with Intel Cyclone V FPGA chip, and ARM Cortex A9 microprocessor. The ARM processor runs a reference Linux kernel image, provided by Rocketboards.

 The programming language used is Verilog HDL for the FPGA, and C++ for Genetics Algorithm program.

## Methodology:

### 1.	Logical Cell & Logical Cell Array (LCA)

<img src="https://imgur.com/S41A63O.png" width="650">

	Figure 1. A Logical Cell (Left) and a 4x4 Logical Cell Array (Right)

A “Logical Cell” (Figure 1, Left) is a simple device with 2-inputs and 1-output, where the output is some logical function of the inputs. Each cell contains a small memory register. The logical function of a cell can be configured by manipulating the memory of the cell. The possible memory configurations are shown in Table 1.

<img src="https://imgur.com/iagD06D.png" width="550">

	Table 1. Possible Logical Cell Configurations

We combine multiple logical cells to create a “Logical Cell Array” (Figure 1, Right). The Logical Cell Array (LCA) forms a grid of cells, each cell taking inputs from two neighboring cells in the next row. Cells on the edges of the grid are connected to cells on the opposite edge, forming a torus shape. Finally, the bottom-most row’s connections are modified; each cell is connected to one input bit from the HPS, and a neighboring cell, instead of two neighboring cells. This design is described in the following section.

Here, y and x denotes the cell’s coordinates within the LCA. DIMY and DIMX is the dimensions of the LCA in the Y and X axis respectively.

<img src="https://imgur.com/A9n6y4y.png">

Each cell’s input is determined by their coordinates, expressed with the following rules:

<img src="https://imgur.com/7JfSUBQ.png">

Here, Ix denotes the xth input bit from the HPS. Inputs from the HPS are set based on the current truth table configuration.

The output bits of the LCA are denoted as Ox, these are the output of the circuit as a whole, to be compared with the truth table’s configuration. Output bits are connected to every cell in the top-most row.

<img src="https://imgur.com/JxHAi6t.png">

In total, any given LCA will have a DIMX-bit input and output signal. For this research, we have implemented a 64x64 LCA.


### 2.	Cellular Automaton

<img src="https://imgur.com/fs1xaYY.png" width="650">

	Figure 2. Converting number matrix to logic circuit.
	1) Using CA rules, we generate a DIMY-by-DIMX matrix.
	2) Write the number matrix onto the LCA memory registers.
	3) The LCA is now configured into some arbitrary logical circuit.

This paper uses Cellular Automaton (CA) as the encoding algorithm for logical circuits. By applying an arbitrary CA rule, we can generate some DIMY-by-DIMX matrix representing the LCA, with each element representing a cell’s state. Writing this matrix into the LCA memory register will configure the LCA into some arbitrary logical circuit. This process is shown in Figure 2.

For this research, we applied a one-dimensional, nearest-neighbor, 4-color CA. Similar to Wolfram’s work on elementary cellular automata, we define the neighbors for each cell ![Cell Definition](https://imgur.com/PR9Cb2K.png) as ![CA Neighborhood definition](https://imgur.com/2mRmXT0.png), where y denotes the vertical axis (time), and x denotes the horizontal axis (cell index). With this settings, CA rules are 4^3=64 integers long, and there are approximately 4^64≈3.403×10^38 rules in total. Finally, the CA grid is torus shaped, cells at edges of the grid loops around to the opposite edges, similar to the LCA.

With this method, we can encode some arbitrary logic circuit as the CA rules required to generate it.


## 3.	Genetic Algorithm

In this paper, we use Genetic Algorithm (GA) to search for desired logical circuits. We define the genotypes as the CA rule, and the phenotypes as the generated circuit. Each individual’s DNA is a CA rule of the format:

![DNA Format](https://imgur.com/JudXNqC.png)

This paper utilizes the following GA strategies: Fitness-Proportionate Selection, Elitism, Tournament Selection, and Uniform Crossover.

-----

## Setup:

 The hardware used for this project is [Terasic's DE0-Nano-SoC rev. C0.](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=941)

 The kit is running the stock Linux image from [Terasic's website.](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=941&PartNo=4)

 FPGA Design Software used is [Quartus Prime Lite 18.1 on Windows](http://fpgasoftware.intel.com/?edition=lite)

 Cross-compiler package used is `gcc-arm-linux-gnueabihf/xenial,now 4:5.3.1-1ubuntu1 amd64`

#### Building

 0. Clear old object files with `make clean`

 1. Cross-compile for ARM Cortex A9 with `make arm`

	*Optionally, compile for PC debugging with `make pc`*

 2. Copy binary file `ga.prog` to the HPS with `scp`

 `scp <local directory>/ga.prog <Username>@<Remote IP>:/<Remote Directory>`

 3. Run the binary file

#### Program Usage

 * Input option numbers to navigate the program's command-line interface

 ![Main menu](https://imgur.com/KgKegfz.png)

 > Main menu of the experimental program.
 >
 > Each option on the menu is accessed by inputting its number.

 &nbsp;

 * Setup preferred Genetic Algorithm parameters in the settings, option **[2]**

 ![Settings Menu](https://i.imgur.com/lIi8Ird.png)

 > Settings Menu. Select a parameter and enter a desired value to modify parameters.

 &nbsp;

 * Load desired truth table with option **[5]**, by providing the path of a truth table file.

 ![Truth Table](https://i.imgur.com/BiSdSTS.png)

 > Provide a path to the truth table file.
 >
 > Truth table files are .csv files, with data in a specific format.

 &nbsp;

 * Run the simulation with option **[6]**

 ![Simulation](https://i.imgur.com/Bd2PRiA.png)

 > Runs a simulation with the provided parameters and truth table.
 >
 > At the end of the simulation, the results will be displayed.

 &nbsp;

 * Inspect DNA strings with option **[8]**

 ![Inspect DNA](https://i.imgur.com/VrqzGLc.png)

 > Generates a Cellular Automaton grid, and a logical circuit, from the provided DNA string.
 >
 > The first 64x64 grid is generated from a seed row, a row of zeros, with a single '1' in the middle. Each row is generated by taking the values of the previous row, and applying the DNA string as a rule Look-Up-Table.
 >
 > The second 64x64 grid is generated using the last row of the previous step as seed. *This is the same thing as generating a 64x128 Cellular Automaton grid from a seed row.*
 >
 > The 64x64 grid generated in step 2 is the converted into a logical circuit, and has its truth table evaluated.
 >
 > The fitness score is given by how close it comes to producing the desired truth table. While the efficiency score is given according to how many NAND gates exist on the final circuit. The less gate exists, the higher the score (less logic gate == higher efficiency).

&nbsp;

Other options include:

 * **[3]** Initialization of program
 * **[4]** Verification of FPGA circuitry.
 * **[7]** Exporting the generated Cellular Automaton grid in CSV and NETPBM format.
 * **[9]** Exporting the previous simulation's report,
 * **[10]** Special preprogrammed routine, for automated testing.

-----

## Preliminary Results:

 The following part represents Cellular Automaton Grids with a colored image.

 <img src="https://imgur.com/PrtCFzD.png" height="250">

 Each color represents a cell, configured to perform some logical function.

 The combination of these "logical cells" form a logic circuit, of some arbitrary function.

 We evaluate randomly generated circuits against a desired truth table. The circuit with the most resemblance to the target is deemed "fitter". Using Genetic Algorithm, we are able to optimize for the "fittest" circuits. The following section contains the preliminary results from our experiment, in trying to synthesize several different logical circuits.

---

### Combinational Logic Circuit Primitives

 The items on the following list are simple primitive Boolean functions, with 2 inputs and 1 output.

 Each item is some Boolean function `f(A,B) = Output`. For each item, the following is provided:

 * The Truth Table (Input A, Input B, Output)
 * The best DNA found (Genetic Algorithm's solution, a Cellular Automaton ruleset)
 * The grid generated using the Cellular Automaton rule (Represented in colors)
 * *The grid converted into a logic circuit - TBD*

 The input is at the bottom of the circuit, in the format: `ABABAB...`

 The output is the top-left-most cell of each circuit.

 The signal flows from the bottom, up to the top, and loops around.

##### 0. FALSE

 A | B | Output    
 --|---|---
 0 | 0 | 0
 0 | 1 | 0
 1 | 0 | 0
 1 | 1 | 0

 DNA: `0030131020030300000101030103033321333200011001030323123330333000`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/4nN9rWy.png" width="250"> | <img src="https://imgur.com/dreslqq.png" width="250">

 &nbsp;

##### 1. NOR (A,B)

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 0
 1 | 0 | 0
 1 | 1 | 0

 DNA: `1112310302033013300011330321010001300101020233003100302310201333`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/0uLc8sz.png" width="250"> | <img src="https://imgur.com/m2p16Zg.png" width="250">

 &nbsp;

##### 2. A & !B

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 1
 1 | 0 | 0
 1 | 1 | 0

 DNA: `1100002031130300201312220023302230033303311001233333310032213332`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/1AEcntI.png" width="250"> | <img src="https://imgur.com/LJ7aXV2.png" width="250">

 &nbsp;

##### 3. !B

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 1
 1 | 0 | 0
 1 | 1 | 0

 DNA: `2200221303031331331011111300002310322101001103333213001000003021`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/NI4c277.png" width="250"> | <img src="https://imgur.com/pccj5bT.png" width="250">

 &nbsp;

##### 4. B & !A

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 0
 1 | 0 | 1
 1 | 1 | 0

 DNA: `3033020222101233100011232030110100003110020002011331310103303000`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/m8RdPsF.png" width="250"> | <img src="https://imgur.com/BdfujJp.png" width="250">

 &nbsp;

##### 5. !A

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 0
 1 | 0 | 1
 1 | 1 | 0

 DNA: `1003133302303202122011110200033131331112302230301100013202013033`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/lSAwuVJ.png" width="250"> | <img src="https://imgur.com/ATshoJo.png" width="250">

 &nbsp;

##### 6. XOR (A,B)

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 1
 1 | 0 | 1
 1 | 1 | 0

 DNA: `3211103103232300010033323212323121223311200103133031331232222230`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/xsogOYH.png" width="250"> | <img src="https://imgur.com/B6M2rr9.png" width="250">

 &nbsp;

##### 7. NAND (A,B)

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 1
 1 | 0 | 1
 1 | 1 | 0

 DNA: `1110122123221030013011032123012133013233203213310033101321322210`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/g6nBAGg.png" width="250"> | <img src="https://imgur.com/lQLSfuL.png" width="250">

 &nbsp;

##### 8. AND (A,B)

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 0
 1 | 0 | 0
 1 | 1 | 1

 DNA: `3021033113110012320331313012003100323333331301331333111030313311`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/7rOQJuC.png" width="250"> | <img src="https://imgur.com/CA1rSRH.png" width="250">

 &nbsp;

##### 9. XNOR (A,B)

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 0
 1 | 0 | 0
 1 | 1 | 1

 DNA: `1333033311302303013231310130233100000103332033331323332330101123`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/qLuBGdy.png" width="250"> | <img src="https://imgur.com/oH1vjqn.png" width="250">

 &nbsp;

##### 10. A

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 1
 1 | 0 | 0
 1 | 1 | 1

 DNA: `0130100231132300110201010323000013332130031033020330302302310033`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/J4FCL4G.png" width="250"> | <img src="https://imgur.com/mfcerUy.png" width="250">

 &nbsp;

##### 11. A + !B

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 1
 1 | 0 | 0
 1 | 1 | 1

 DNA: `1133331200232301201111213301331313310033013301203100021021133333`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/D5Ae6V9.png" width="250"> | <img src="https://imgur.com/qn95263.png" width="250">

 &nbsp;

##### 12. B

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 0
 1 | 0 | 1
 1 | 1 | 1

 DNA: `1131020211002321222211101130323132113212131321112332212213302103`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/88D6onS.png" width="250"> | <img src="https://imgur.com/4p8yRin.png" width="250">

 &nbsp;

##### 13. B + !A

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 0
 1 | 0 | 1
 1 | 1 | 1

 DNA: `0330230333032111013021110131110313021033303033332133310001012330`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/73PWgei.png" width="250"> | <img src="https://imgur.com/VzvKd5i.png" width="250">

 &nbsp;

##### 14. OR (A,B)

 A | B | Output
 --|---|---
 0 | 0 | 0
 0 | 1 | 1
 1 | 0 | 1
 1 | 1 | 1

 DNA: `3330103330320031330033121213112111030111101203003333312111101231`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/IzLQQWS.png" width="250"> | <img src="https://imgur.com/ZTSkPf1.png" width="250">

 &nbsp;

##### 15. TRUE

 A | B | Output
 --|---|---
 0 | 0 | 1
 0 | 1 | 1
 1 | 0 | 1
 1 | 1 | 1

 DNA: `2201112321302333231030320300003003233320222020230323032221110320`

 Cellular Automaton Grid | Logical Circuit
 ------------------------|----------------
 <img src="https://imgur.com/ZfTHkJc.png" width="250"> | <img src="https://imgur.com/xi4W04p.png" width="250">

 &nbsp;

---

### Sequential Logic Circuit

 The items on the next list are sequential logic circuit synthesis results.

#### SR NAND Flip-Flip (Active Low Signal)

S | R | Action
--|---|---
0 | 0 | PROHIBITED
0 | 1 | RESET
1 | 0 | SET
1 | 1 | LACTH

DNA:
`2023331301333313231333320020333322331322232030300013221033221321`
`2030303032023312230303022323010320130333202210331032331313233233`
`2023312101300002110021131031012111303121022133130300311320100200`


Cellular Automaton Grid | Logical Circuit
------------------------|----------------
<img src="https://imgur.com/0FSL6Nr.png" width="250"> | <img src="https://imgur.com/2tRZPAK.png" width="250">
<img src="https://imgur.com/TfGwe74.png" width="250"> | <img src="https://imgur.com/LsMYpaK.png" width="250">
<img src="https://imgur.com/zb4UHFf.png" width="250"> | <img src="https://imgur.com/3g9gCcT.png" width="250">

&nbsp;

#### SR NOR Flip-Flip (Active High Signal)

S | R | Action
--|---|---
0 | 0 | LATCH
0 | 1 | RESET
1 | 0 | SET
1 | 1 | PROHIBITED

DNA:
`3032203331220033220300013031213231003130301030023200010203032333`
`2132330201331002200013100312031000302103332212032331332330303331`
`0030033100031333323030030322323320210313300301031002333200133123`

Cellular Automaton Grid | Logical Circuit
------------------------|----------------
<img src="https://imgur.com/cw1pN2i.png" width="250"> | <img src="https://imgur.com/s23SlPf.png" width="250">
<img src="https://imgur.com/6TrB5L5.png" width="250"> | <img src="https://imgur.com/ET9oeFe.png" width="250">
<img src="https://imgur.com/TYReRtp.png" width="250"> | <img src="https://imgur.com/7Z7A3uR.png" width="250">

&nbsp;
