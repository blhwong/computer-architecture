4-lc-3b-interrupt-and-exception-handler
=======================================
LC-3b with Interrupt and Exception Handler
Created by: Brandon Wong

In this project, we are to modify the LC-3b architecture to accommodate interrupt and exceptions.
I was to modify the control store microcode with additional bits to accommodate the states that 
are part of the interrupt handler. I modified the LC-3b microarchitecture and added extra paths, 
and gates to support my interrupt and exception handler design. I modified the microsequencer to
also support my design of the interrupt and exception handler for the LC-3b architecture. You can
see these modifications and drawings on the pdf labeled Modifications which includes my diagrams 
of the modified state machine, microsequencer, and microarchitecture. Below is also more explanation
on how my design works.



1. The state machine will begin normally at 18, 19. Whenever MAR is loaded, it must check to see if it is an address that 
is aligned and not between 0x0000 and 0x2fff. So for states 18, 19, 2, 6, 7, and 3 are all modified so that it does a routine 
to check MAR. The J for states 18, 19, 2, 6, 7 are still its original next states of 33, 33, 29, 25, 23, and 24 respectively 
in the control store file, but its respective next state is saved before it checks for MAR at state 48. At state 48, it will 
check MAR and if there is an interrupt or an exception, the INT flag will be set. State 49 will not do anything except to check 
where the INT goes to. If there is an interrupt, it will go to state 58 where it sets the Next state to the INT_STATE which is 
defined as state 63. If there is not an interrupt, it will go to state 50, where it will clear the next state, and then it will
branch back to its original next state( i.e. State 18, 19 will be back to its original next state of 33 after it does the 
Check MAR routine).  

The designated INT_State 63 will begin by setting mode to 0 for the supervisor mode, it will put the current PSR into MDR, 
it will VEC <- INTV + EXCV (note that for every detection of interrupts, it will set INTV and clear the EXCV or vice versa), 
then it loads PC <- PC - 2 since states 18, 19 had incremented the PC, then in state 26 it will do the stack switching by 
saving the USP<- SP, and by loading SP<- SSP. State 10 will decrement the SP <- SP -2 and also put it in MAR <- SP-2. 
In state 36, it will save PSR by M[MAR] <- MDR and whenever memory is ready it will go onto the next state. Then it will 
repeat the same thing to push PC into the SSP for states 38, 11, and 40. After it has pushed PC, it will load
MAR <- Interrupt Exception Base Register (IEBR) + LSHF(vec, 1) by GateVEC. It will go into memory to find the address
of the interrupt handler, and then load that address into the PC to where it will go back to state 18 to begin the interrupt handling. 
After finishing the handler of the interrupt, State 32 will detect an RTI and lead it to state 8. State 8 will put the SP into MAR 
and save the Next<- J because it has to the routine of checking MAR. If the mode is set to 1, and you are trying to do RTI, the check
MAR routine will catch that because you are accessing the System stack pointer as a user. In that case, it will cause a protection
exception and it will interrupt. If the Mode is indeed 0, then it will go through the check MAR and end up in state 37. In state 37, 
it will go into memory to retrieve PC that was saved in the SP and when it’s ready it will go to state 39. State 39 will Load PC<-MDR.
In state 47, it will increment both MAR, SP <- SP+2 to prepare to go into memory again. In state 41 it will go into memory to retrieve 
PSR, and when it’s ready it will go to state 43. State 43 will Load PSR <- MDR. In state 51, it will increment SP <- SP+2. In state 61, 
it will implement the stack switching by saving SSP, and loading SP<- USP to where it will go back to state 18. 
State 32 will also be responsible for detecting the unknown opcode, and in that case it will set the respective EXCV and then 
branch to state 63 to handle its exception.

2. Some of the additions of the data path are LD.mode, and ld.psr. if you decide to ld. Mode then you will only be setting mode =0. 
If you are loading psr, then you will have to take the contents of the bus and RSHF by 15 so that a single bit can be put into mode. 
When gating PSR, it will do the same by Left Shifting it and adding it to the Bus. 
Loading VEC will be just the sum of INTV and EXCV. When it comes to gating the VEC, it will have to LSHF VEC and add it will IEBR.
Loading PSR you will have to do some logic when setting the NZP bits, but it goes hand in hand with LD.CC. Gating the PSR however, 
you will have to take LSHF(N, 2), LSHF(Z, 1), P and then add it onto the bus. Also, the control unit will determine what the next 
state is when you decide to LD.NEXT
The REG file also contains a new LD.USP, and LD.SP, and inside the REG file contains an SPMUX much similar to the SR1MUX which is 
hidden inside of the REG file, but I will explain this in the next section

3. I added an extra bit for the condition to determine the next INT state which is obvious in the microsequencer. I increased 
the DRMUX to 2 bits so that when DRMUX = 10, it will indicate the SP as the destination reg. I increased the SR1mux to 2 bits so 
that when SR1MUX = 10, it will be the results of SPMUX, if SR1MUX = 11, it will be SP.
SPMUX can be 00, 01, 10, or 11, which corresponds to SP + 2, SP -2, or SSP respectively. 
I also added a ld.mode. you will only need to set it to supervisor mode so ld.mode will only be set to 0. Ld. Vec which will ld the 
sum of INTV + EXCV, Ld.USP and LD.SSP is for saving their stack pointers, LD.NEXT which will save its next state for those states 
that have to check for MAR, a GATEVEC which I’ve explained earlier, and GATE.PSR which I also explained above in the data path. 

4. The changes to the microsequencer, I’ve added an extra condition so it will determine a different state if an INT is present, 
in that case I would have to set the conditions to 100 which will determine J. If IRD =1, it will have to determine the next state 
a different way. The next state when IRD =1 is also determined by the condition bits. If COND = 00, 01, or 10, and COND = 11 is unused. 
If the conditions are 00, then it will do the normal 0,0, IR[15, 12]. If conditions are 01, then it will have to go through the 
check_state so the next state is 48 which is the routine of checking MAR. if the conditions are 10, then it will load the NEXT state 
that was saved right before going into the check_state. 
The microsequencer also determines setting the Interrupts, setting the corresponding EXCV, and INTV. If there is an timer interrupt, 
it will set INTV and clear the EXCV. If there is an exception, it will set EXCV, and clear INTV.
