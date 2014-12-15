5-lc-3b-virtual-memory
======================
LC-3b Virtual Memory accommodation
Created by: Brandon Wong

Very similar to the 4th modification of the LC-3b with accommodation of interrupt and exception handling.
Except that this project is everything plus the accommodation of virtual memory translation. This takes
a 16 bit wide physical address and does a simple one level translation into a 32 bit virtual address.
We are to also handle an additional exception from page fault translations. Added additional states for
the microcode labeled ucode5 to handle the address translations.

The translation is as follows:

1. Save the MAR into the temporary register VA
2. Load the MAR with the address of the PTE of the page containing the VA:
	MAR[15:8] ? PTBR[15:8]
	MAR[7:0] ? LSHF(VA[15:9], 1)
3. i.e., MAR ? PTBR + (2 × page_number).
4. Read the PTE of the page containing the VA into the MDR
5. Check for a protection exception
6. Check for a page fault
7. Set the reference bit of the PTE
8. If the pending access is a write, set the modified bit of the PTE
9. Write the PTE back to memory
10. Load the physical address into the MAR:
	MAR[13:9] ? PFN
	MAR[8:0] ? VA[8:0]
11. If the operation is a write, load the MDR with the source register

