# Generation of Logical Circuits using Genetics Algorithm and Cellular Automata

This research project's main objective is to automatically synthesize arbitrary logical circuits,
using genetics algorithm and cellular automata.

**Brief Methodology Explanation:**

This research's logical circuits are designed to be analogous to cellular automata.

An array of logical cells are connected to their neighbors, and can perform some logical function.
Each cell takes inputs from their neighbors, and outputs to their neighbors.
Together, the cell forms some arbitrary logical circuit. We call this device the *Logical Cell Array*.

Cellular Automata is used to generate a setting for the logical cell array,
which is equivalent to using CA to generate random logical circuits.
Meanwhile, we use Genetics Algorithm to optimize/search for some desired circuit,
and by extension, some desirable Cellular Automata rule.

**See [the wiki](https://github.com/mimocha/ga-logic-circuit/wiki) for more info.**

-----

### Brief Usage Explanation:

#### Programming the FPGA

Compile FPGA project in Quartus Prime.

* Programming via JTAG
	1. Set the DE0-Nano-SoC's MSEL switch to `101010`
	2. Using *Quartus Programmer*, program the FPGA with a `.sof` file.

Refer to Terasic's [*DE0-Nano-SoC - User Manual*](https://www.terasic.com.tw/attachment/archive/941/DE0-Nano-SoC_User_manual_rev.C1.pdf) (chapter 3) for a step-by-step guide.

* Programming via EPCS
	1. Set the DE0-Nano-SoC's MSEL switch to `101100`
	2. Convert a `.sof` to a `.jic` file.
	3. Using *Quartus Programmer*, program the EPCS device with a `.jic` file

Refer to Terasic's [*DE0-Nano-SoC - User Manual*](https://www.terasic.com.tw/attachment/archive/941/DE0-Nano-SoC_User_manual_rev.C1.pdf) (chapter 8) for a step-by-step guide.

#### Using the GA Program

0. Clear old object files with `make clean`

1. Cross-compile for ARM Cortex A9 with `make arm`

 *Or compile for PC debugging with* `make pc`

2. Copy binary file `ga.prog` to DE0-nano-SoC with `scp`

 `scp <local directory>/ga.prog <Username>@<Remote IP>:/<Remote Directory>`

3. Run the binary file in terminal

4. Navigate the program's CLI by entering option numbers.

-----

### Results Example:

Using Cellular Automata, we generate a 64x64 integer matrix of range [0,3].
This number matrix represents a setting for the logical cell array,
and by extension, some arbitrary logical circuit.
We can visualize this number matrix, by creating a 64x64 image,
where each pixel represents a cell, and each color represents a cell state.

<img src="https://i.imgur.com/IzLQQWS.png" width="500">

`Visualized numerical matrix, which forms a logical OR gate`

Here, each color represents the following:

Color | Circuit Equivalence
------|--------------------
Black | GND (Cell outputs constant 0)
Cyan | Straight Wire (Cell passes its input 1)
Magenta | Slanted Wire (Cell passes its input 2)
Yellow | NAND Gate (Cell NANDs its input 1 & 2)

The above visualization can be converted to the following circuit image:

<img src="https://i.imgur.com/ZTSkPf1.png" width="500">

`Converted logical circuit image, which forms a logical OR gate`
