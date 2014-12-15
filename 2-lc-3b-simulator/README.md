2-lc-3b-simulator
=================
LC-3B Simulator
Created by: Brandon Wong and Duc Tran

take one input file which is an assembled LC3-b program. The simulator will execute
the input LC-3b program, one instruction at a time, modifying the architectural state
of the LC-3b after each instruction.

We are to modify the states which means to change the state of the registers, condition codes,
and memory based on the LC-3b architecture. We must latch the data values, and follow the
proper arithmetic based on the LC-3b ISA. You may reference the ISA on the pdf labeled
LC-3b ISA.