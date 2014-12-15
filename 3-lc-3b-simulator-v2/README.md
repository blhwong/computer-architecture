3-lc-3b-simulator-v2
====================
LC-3b modified cycle level simulator
Created by: Brandon Wong

Very similar to the original simulator, but it is a cycle level simulator.
Rather than just the original instruction level simulator, it will traverse through
the LC-3b state machine. The states contain a 35-bit wide microcode in which you can find
labeled ucode3. The microcode drives the microsequencer and evaluates using multiplexers to
determine which state to go next. We are to individually write the functions to evaluate
microsequencer, cycle memory, evaluate bus drivers, drive bus, and latch data path values.
I followed the state machine based on the pdf labeled LC-3b State Machine, and the microarchitecture
based on the pdf labeled LC-3b Microarchitecture.