# Generation of Logic Circuits with Genetics Algorithm
## An Examination in Physical Implementation

-----

## Background:

This repository is for programming files for my bachelor degree's dissertation project.

The main objective for this dissertation is to research the application of evolutionary hardware, and implementing genetics algorithm and cellular automaton to generate logical circuits.

The research is done on the Terasic DE0-Nano-SoC development board. The board is equipped with Intel Cyclone V FPGA chip, and ARM Cortex A9 microprocessor.

Thus the programming language used is Verilog HDL for the FPGA, and C++ for Genetics Algorithm program.

-----

## Setup:

The hardware used for this project is [Terasic's DE0-Nano-SoC rev. C0.](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=941)

The kit is running the stock Linux image from [Terasic's website.](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&CategoryNo=205&No=941&PartNo=4)

FPGA Design Software used is [Quartus Prime Lite 18.1 on Windows](http://fpgasoftware.intel.com/?edition=lite)

Cross-compiler package used is `gcc-arm-linux-gnueabihf/xenial,now 4:5.3.1-1ubuntu1 amd64`

-----

## Usage

#### Build & Executing

1. Cross-compile for ARM Cortex A9 with `make arm`
2. Copy binary file `ga.prog` to the HPS with `scp`

 `scp <local directory>/ga.prog <Username>@<HPS Local IP>:/home/<Username>/`

3. Run the binary file on the HPS
4. Clear old object files with `make clean`

#### Program Usage

* Input option numbers to navigate the program.
