/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Brandon Wong
    Name 2: Duc Tran
    UTEID 1: blw868
    UTEID 2: dmt735
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_EN 1
/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/
#define START 0x01800
#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%0.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%0.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

enum OPCODE {
	BR,
	ADD,
	LDB,
	STB,
	
	JSR,
	AND,
	LDW,
	STW,
	
	RTI, /***IGNORE***/
	XOR,
	NOTUSE1,
	NOTUSE2,
	
	JMP,
	SHF,
	LEA,
	TRAP
	
};
void readAndParse(int msb, int lsb, int style,int* a1, int* a2, int* a3){
	/*
	*Parse assembled code to : opcode arg1 arg2 arg3
	*Style:
	*	
		0:
			OPCODE DR SR1 SR2
		1:
			OPCODE DR SR1 IMM5
		2:
			OPCODE NZP/DR PCOFFSET9
		3: 
			OPCODE PCOFFSET11
		4:	
			OPCODE BASE_R
		5:
			OPCODE DR BASE_R OFFSET_6
		6:
			OPCODE TRAP8
		7:
			OPCODE DR SR AMOUNT4
		
	*/
	int op;
	op = msb >> 4;
	*a1 = 0;
	*a2 = 0;
	*a3 = 0;
	switch(style)
	{
		case 0:
			*a1 = (msb >> 1)& 0x07;	/* DR */
			*a2 = ((msb & 0x01) << 2) | (lsb>> 6);/*SR1 :: BASER*/
			*a3 = lsb & 0x07;
			break;
		case 1:
			*a1 = (msb >> 1)& 0x07;
			*a2 = ((msb & 0x01) << 2) | (lsb>> 6);
			*a3 = (lsb & 0x1F);
			break;
		case 2:
			*a1 = (msb >> 1)& 0x07;
			*a2 = lsb + ((msb & 0x1) << 8);
			break;
		case 3:
			*a1 = ((msb & 0x07) << 8) + lsb;
			break;
		case 4:
			*a1 = ((msb & 0x01) << 2) | (lsb>> 6);
			break;
		case 5:
			*a1 = (msb >> 1)& 0x07;	/* DR */
			*a2 = ((msb & 0x01) << 2) | (lsb>> 6);/*SR1 :: BASER*/
			*a3 = (lsb & 0x3F);
			break;
		case 6:
			*a1 = lsb;
			break;
		case 7:
			*a1 = (msb >> 1)& 0x07;	/* DR */
			*a2 = ((msb & 0x01) << 2) | (lsb>> 6);/*SR1 :: BASER*/
			*a3 = (lsb & 0x0F);
			break;	
	
	}	
}

void setcc(int value)
{
	NEXT_LATCHES.N = 0;
	NEXT_LATCHES.Z = 0;
	NEXT_LATCHES.P = 0;
	if(Low16bits(value) >> 15)
	{
		NEXT_LATCHES.N = 1;
		return;
	}
	if(Low16bits(value) == 0)
	{
		NEXT_LATCHES.Z = 1;
		return;
	}
	else
	{
		NEXT_LATCHES.P = 1;
		return;
	}
	
}

void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */   
    
	int i,msb, lsb,style;
    int opcode;
	int arg1,arg2,arg3;
	
    /*if(CURRENT_LATCHES.PC  > 0x08000)
		exit(0);*/
		
	msb = MEMORY[CURRENT_LATCHES.PC >> 1][1];
	lsb = MEMORY[CURRENT_LATCHES.PC >> 1][0];
	if(DEBUG_EN)
		printf("MSB: %x LSB: %x \n" , msb,lsb);
		
	/*Begins decoding here*/	
	/*Check for boundary*/
	
	/*Increment PC*/
	NEXT_LATCHES.PC = Low16bits((CURRENT_LATCHES.PC) + 2); /* assumes pc overflow back to 0 */
	
	opcode = msb >> 4;
	switch(opcode){
		case ADD: 
			if(DEBUG_EN)
			{
				printf(" ADD instruction found \n");
			}
			if((lsb & 0x20) >> 5) /*Add Imm5*/
			{
				readAndParse(msb,lsb,1,&arg1,&arg2,&arg3);
				if(Low16bits(arg3)>>4)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] + (Low16bits(arg3) + 0xFFE0));  /* sign extend */
					
				}
				else
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] + Low16bits(arg3));
				}
				
				
			}else
			{
				readAndParse(msb,lsb,0,&arg1,&arg2,&arg3);
				NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] + CURRENT_LATCHES.REGS[arg3]);
			}
				
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			
			/*set condition code*/
			setcc(Low16bits(NEXT_LATCHES.REGS[arg1]));
			
			break;
		case AND:
			if(DEBUG_EN)
			{
				printf(" AND instruction found \n");
			}
			if((lsb & 0x20) >> 5) /*Add Imm5*/
			{
				readAndParse(msb,lsb,1,&arg1,&arg2,&arg3);
				if(Low16bits(arg3)>>4)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] & (Low16bits(arg3) + 0xFFE0));
				}
				else
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] & Low16bits(arg3));
				}
			}else
			{
				readAndParse(msb,lsb,0,&arg1,&arg2,&arg3);
				NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] & CURRENT_LATCHES.REGS[arg3]);
			}
			
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			/* **** set cc ****/
			setcc(Low16bits(NEXT_LATCHES.REGS[arg1]));
			break;
		case BR:
			if(DEBUG_EN)
			{
				printf(" BR instruction found \n");
			}
			readAndParse(msb,lsb,2,&arg1,&arg2,&arg3);
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			
			if((CURRENT_LATCHES.N & arg1 >> 2) || (CURRENT_LATCHES.Z & arg1 >> 1) || (CURRENT_LATCHES.P & arg1 >> 0) )
			{
				if(Low16bits(arg2) >> 8)
				{
					NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + ((Low16bits(arg2) + 0xFE00)<<1));
				}
				else
				{
					NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (Low16bits(arg2)<<1));
				}
			}
			break;
		case JMP:
			/*RET*/
			if(DEBUG_EN)
			{
				printf(" JMP instruction found \n");
			}
			readAndParse(msb,lsb,4,&arg1,&arg2,&arg3);
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			if(CURRENT_LATCHES.REGS[arg1] % 2 != 0)
			{
				printf("Illegal operand address exception occurs -JMP/RET");
			
			}else
			{
				NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[arg1]);
			}
			break;
			
		case JSR:
		
			if(DEBUG_EN)
			{
				printf(" JSR instruction found \n");
			}
			NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
			if((msb & 0x08) >> 3) /*Imm11*/
			{
				readAndParse(msb,lsb,3,&arg1,&arg2,&arg3);
				if(arg1>>10)
				{
					NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC +  Low16bits((arg1+0xF800)<<1));
					if(DEBUG_EN)
						printf("%x", NEXT_LATCHES.PC);
					
				}else
				{
					NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + Low16bits(((arg1<<1))));
				}
			}else
			{
				readAndParse(msb,lsb,4,&arg1,&arg2,&arg3);
				/*must contain a word address , if odd illegal operand address exception occurs*/
				if(CURRENT_LATCHES.REGS[arg1]%2 !=0)
				{
					printf("Illegal operand address exception occurs - JSRR");
				}else
				{
					NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[arg1]);
				}
			}	
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			break;
		case LDB:
			if(DEBUG_EN)
			{
				printf("LDB instruction found \n");
			}

				readAndParse(msb,lsb,5,&arg1,&arg2,&arg3);

			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
																				/* assume we don't care about msb */
			if(Low16bits(arg3) >> 5)
			{
				int a;
				a = Low16bits(CURRENT_LATCHES.REGS[arg2] + (Low16bits(arg3 + 0xFFC0)));
				if(MEMORY[a>>1][a&0x0001] >>15)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(MEMORY[a>>1][a&0x0001] + 0xFF00); /*sign extended*/
				}else
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(MEMORY[a>>1][a&0x0001]);
				}
			}
			else
			{
				int a;
				a = Low16bits(CURRENT_LATCHES.REGS[arg2] + (Low16bits(arg3)));
				if(MEMORY[a>>1][a&0x0001] >>15)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(MEMORY[a>>1][a&0x0001] + 0xFF00); /*sign extended*/
				}else
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(MEMORY[a>>1][a&0x0001]);
				}

			}
			
			/* set cc */
			setcc(Low16bits(NEXT_LATCHES.REGS[arg1]));
			break;
		case LDW:
			if(DEBUG_EN)
			{
				printf("LDW instruction found \n");
			}

				readAndParse(msb,lsb,5,&arg1,&arg2,&arg3);

			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			
			
			if(Low16bits(arg3) >> 5)
			{
				int a;
				a = Low16bits((CURRENT_LATCHES.REGS[arg2]) + Low16bits((arg3 + 0xFFC0) << 1));
				if(a%2 == 0)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(MEMORY[a>>1][0]) +Low16bits(MEMORY[a>>1][1] << 8 ) ;
				}
				else
				{
					printf("reading from an odd address\n");
				}
			}
			else
			{
				int a;
				a = Low16bits((CURRENT_LATCHES.REGS[arg2]) + Low16bits((arg3) << 1));
				if(a%2 == 0)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(MEMORY[a>>1][0]) +Low16bits(MEMORY[a>>1][1] << 8 ) ;
				}
				else
				{
					printf("reading from an odd address\n");
				}
			}
			
			
			/* set cc */
			setcc(Low16bits(NEXT_LATCHES.REGS[arg1]));
			break;
		case LEA:
		
			if(DEBUG_EN)
			{
				printf("LEA instruction found \n");
			}

				readAndParse(msb,lsb,2,&arg1,&arg2,&arg3);

			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			

			if(arg2 >> 8)
			{
				NEXT_LATCHES.REGS[arg1] = Low16bits(NEXT_LATCHES.PC +(Low16bits(0xFE00 + arg2) << 1));
			}
			else
			{
				NEXT_LATCHES.REGS[arg1] = Low16bits(NEXT_LATCHES.PC +Low16bits(arg2 << 1));
			}
				
			break;
		case RTI: /*ignore not doing RTI*/
			break;
		case SHF:
			if(DEBUG_EN)
			{
				printf(" SHF instruction found \n");
			}
			readAndParse(msb,lsb,7,&arg1,&arg2,&arg3);
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			
			if(((lsb & 0x10) >> 4)==0) 			/* LSHF */
			{
				NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2]  << arg3);
			}
			else  	
			{
				if(((lsb & 0x20) >> 5)==0)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2]  >> arg3);	/* RSHFL: DR = RSHF(SR, amount4, 0); */
				}
				else
				{
					int j;
					NEXT_LATCHES.REGS[arg1] = Low16bits((CURRENT_LATCHES.REGS[arg2] >> arg3 ));
					for (j=0;j<arg3;j++)
					{
						NEXT_LATCHES.REGS[arg1] =  Low16bits(NEXT_LATCHES.REGS[arg1] + ((CURRENT_LATCHES.REGS[arg2] & 0x8000)>> j));  /* RSHFA: DR = RSHF(SR, amount4, SR[15]);*/
					}
				}
				
			}
			
			/*set condition code*/
			setcc(Low16bits(NEXT_LATCHES.REGS[arg1]));
			break;
		case STB:
			if(DEBUG_EN)
			{
				printf("STB instruction found \n");
			}
			readAndParse(msb,lsb,5,&arg1,&arg2,&arg3);
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			if(Low16bits(arg3) >> 5)
			{
				int a;
				a = Low16bits(CURRENT_LATCHES.REGS[arg2] + (Low16bits(arg3 + 0xFFC0)));
				MEMORY[a>>1][a&0x0001] = CURRENT_LATCHES.REGS[arg1] & 0x00FF;
			}
			else
			{
				int a;
				a = Low16bits(CURRENT_LATCHES.REGS[arg2] + (Low16bits(arg3)));
				MEMORY[a>>1][a&0x0001] = CURRENT_LATCHES.REGS[arg1] & 0x00FF;
			}
			break;
		case STW:
			if(DEBUG_EN)
			{
				printf("STW instruction found \n");
			}
			readAndParse(msb,lsb,5,&arg1,&arg2,&arg3);
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			
			if(Low16bits(arg3) >> 5)
			{
				int a;
				a = Low16bits(CURRENT_LATCHES.REGS[arg2] + Low16bits((arg3 + 0xFFC0)<<1));
				if(a%2 == 0)
				{
					MEMORY[a>>1][0] = CURRENT_LATCHES.REGS[arg1] & 0x00FF;
					MEMORY[a>>1][1] = (CURRENT_LATCHES.REGS[arg1] &0xFF00)>>8;
				}
				else
				{
					printf("reading from an odd address\n");
				}
			}
			else
			{
				int a;
				a = Low16bits(CURRENT_LATCHES.REGS[arg2] + (Low16bits(arg3 << 1)));
				if(a%2 == 0)
				{
					MEMORY[a>>1][0] = CURRENT_LATCHES.REGS[arg1] & 0x00FF;
					MEMORY[a>>1][1] = (CURRENT_LATCHES.REGS[arg1] &0xFF00)>>8;
				}
				else
				{
					printf("reading from an odd address\n");
				}
			}
			break;
		case TRAP:
			if(DEBUG_EN)
			{
				printf("TRAP instruction found \n");
			}
			readAndParse(msb,lsb,6,&arg1,&arg2,&arg3);
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
			NEXT_LATCHES.PC = (MEMORY[Low16bits(arg1)][1] << 8) + (MEMORY[Low16bits(arg1)][0]);
		
			break;
		case XOR:
			if(DEBUG_EN)
			{
				printf("XOR instruction found \n");
			}
			if((lsb & 0x20) >> 5) /*Add Imm5*/
			{
				readAndParse(msb,lsb,1,&arg1,&arg2,&arg3);
				if(Low16bits(arg3)>>4)
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] ^ Low16bits(arg3 + 0xFFE0));
				}
				else
				{
					NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] ^ Low16bits(arg3));
				}
			}
			else
			{
				readAndParse(msb,lsb,0,&arg1,&arg2,&arg3);
				NEXT_LATCHES.REGS[arg1] = Low16bits(CURRENT_LATCHES.REGS[arg2] ^ CURRENT_LATCHES.REGS[arg3]);
			}
				
			if(DEBUG_EN)
			{
				printf(" %x | %x  | %x \n", arg1, arg2, arg3);
			}
			
			
			/*set condition code*/
			
			setcc(Low16bits(NEXT_LATCHES.REGS[arg1]));
			break;
		
		default:
			if(DEBUG_EN)
				printf(" Unknown opcode \n");	
	}
	
	if(msb == 0xF0 && lsb == 0x25)
	{
		if(DEBUG_EN)
			printf(" HALT \n");
		NEXT_LATCHES.PC = 0x0000;
	}
	
	
}
