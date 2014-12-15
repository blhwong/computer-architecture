6-pipelined-lc-3b-architecture
==============================
Pipelined LC-3b Architecture
Created by: Brandon Wong

This is different from everything up until the 5th project of the LC-3b.
We essentially start from scratch and design a pipelined architecture for 
the LC-3b. In the previous labs, we added several extra states to handle 
interrupt and exceptions, and even more states to accommodate for virtual memory
translations. The old architecture will not fetch the next instruction for evaluation
until the previous instruction is complete. However, with the pipelined architecture,
we can fetch instructions and run it through the pipeline as long as there are no 
dependencies. If there are dependencies, then the architecture will handle it so 
that it will evaluate the program as quickly is possible. This pipelined architecture 
drastically improves the efficiency of the previous LC-3b architecture. I programmed the
architecture based on the pdf labeled Pipelined LC-3b. The differences in the microcode
is also recreated and can be referenced on ucode6