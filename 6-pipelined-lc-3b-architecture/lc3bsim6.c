/*
    Name 1: Brandon Wong
    UTEID 1: blw868
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator - Lab 6                                   */
/*                                                             */
/*   EE 460N -- Spring 2013                                    */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/
void FETCH_stage();
void DE_stage();
void AGEX_stage();
void MEM_stage();
void SR_stage();
/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define TRUE  1
#define FALSE 0

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
/* control signals from the control store */
enum CS_BITS {
  SR1_NEEDED,
  SR2_NEEDED,
  DRMUX,
  
  ADDR1MUX,
  ADDR2MUX1, ADDR2MUX0, 
  LSHF1,
  ADDRESSMUX,
  SR2MUX, 
  ALUK1, ALUK0,
  ALU_RESULTMUX,

  BR_OP,
  UNCOND_OP,
  TRAP_OP,
  BR_STALL,
  
  DCACHE_EN,
  DCACHE_RW,
  DATA_SIZE,

  DR_VALUEMUX1, DR_VALUEMUX0,
  LD_REG,
  LD_CC,
  NUM_CONTROL_STORE_BITS

} CS_BITS;


enum AGEX_CS_BITS {

  AGEX_ADDR1MUX,
  AGEX_ADDR2MUX1, AGEX_ADDR2MUX0, 
  AGEX_LSHF1,
  AGEX_ADDRESSMUX,
  AGEX_SR2MUX,
  AGEX_ALUK1, AGEX_ALUK0, 
  AGEX_ALU_RESULTMUX,

  AGEX_BR_OP,
  AGEX_UNCOND_OP,
  AGEX_TRAP_OP,
  AGEX_BR_STALL,
  AGEX_DCACHE_EN,
  AGEX_DCACHE_RW,
  AGEX_DATA_SIZE,
  
  AGEX_DR_VALUEMUX1, AGEX_DR_VALUEMUX0,
  AGEX_LD_REG,
  AGEX_LD_CC,
  NUM_AGEX_CS_BITS
} AGEX_CS_BITS;

enum MEM_CS_BITS {
  MEM_BR_OP,
  MEM_UNCOND_OP,
  MEM_TRAP_OP,
  MEM_BR_STALL,
  MEM_DCACHE_EN,
  MEM_DCACHE_RW,
  MEM_DATA_SIZE,

  MEM_DR_VALUEMUX1, MEM_DR_VALUEMUX0,
  MEM_LD_REG,
  MEM_LD_CC,
  NUM_MEM_CS_BITS
} MEM_CS_BITS;

enum SR_CS_BITS {
  SR_DR_VALUEMUX1, SR_DR_VALUEMUX0,
  SR_LD_REG,
  SR_LD_CC,
  NUM_SR_CS_BITS
} SR_CS_BITS;


/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int Get_SR1_NEEDED(int *x)     { return (x[SR1_NEEDED]); }
int Get_SR2_NEEDED(int *x)     { return (x[SR2_NEEDED]); }
int Get_DRMUX(int *x)          { return (x[DRMUX]);}
int Get_DE_BR_OP(int *x)       { return (x[BR_OP]); } 
int Get_ADDR1MUX(int *x)       { return (x[AGEX_ADDR1MUX]); }
int Get_ADDR2MUX(int *x)       { return ((x[AGEX_ADDR2MUX1] << 1) + x[AGEX_ADDR2MUX0]); }
int Get_LSHF1(int *x)          { return (x[AGEX_LSHF1]); }
int Get_ADDRESSMUX(int *x)     { return (x[AGEX_ADDRESSMUX]); }
int Get_SR2MUX(int *x)          { return (x[AGEX_SR2MUX]); }
int Get_ALUK(int *x)           { return ((x[AGEX_ALUK1] << 1) + x[AGEX_ALUK0]); }
int Get_ALU_RESULTMUX(int *x)  { return (x[AGEX_ALU_RESULTMUX]); }
int Get_BR_OP(int *x)          { return (x[MEM_BR_OP]); }
int Get_UNCOND_OP(int *x)      { return (x[MEM_UNCOND_OP]); }
int Get_TRAP_OP(int *x)        { return (x[MEM_TRAP_OP]); }
int Get_DCACHE_EN(int *x)      { return (x[MEM_DCACHE_EN]); }
int Get_DCACHE_RW(int *x)      { return (x[MEM_DCACHE_RW]); }
int Get_DATA_SIZE(int *x)      { return (x[MEM_DATA_SIZE]); } 
int Get_DR_VALUEMUX1(int *x)   { return ((x[SR_DR_VALUEMUX1] << 1 ) + x[SR_DR_VALUEMUX0]); }
int Get_AGEX_LD_REG(int *x)    { return (x[AGEX_LD_REG]); }
int Get_AGEX_LD_CC(int *x)     { return (x[AGEX_LD_CC]); }
int Get_MEM_LD_REG(int *x)     { return (x[MEM_LD_REG]); }
int Get_MEM_LD_CC(int *x)      { return (x[MEM_LD_CC]); }
int Get_SR_LD_REG(int *x)      { return (x[SR_LD_REG]); }
int Get_SR_LD_CC(int *x)       { return (x[SR_LD_CC]); }
int Get_DE_BR_STALL(int *x)    { return (x[BR_STALL]); }
int Get_AGEX_BR_STALL(int *x)  { return (x[AGEX_BR_STALL]); }
int Get_MEM_BR_STALL(int *x)   { return (x[MEM_BR_STALL]); }



/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][NUM_CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/
/* The LC-3b register file.                                      */
/***************************************************************/
#define LC3b_REGS 8
int REGS[LC3b_REGS];
/***************************************************************/
/* architectural state */
/***************************************************************/
int  PC,  	/* program counter */
     N,		/* n condition bit */
     Z = 1,	/* z condition bit */
     P;		/* p condition bit */ 
/***************************************************************/
/* LC-3b State info.                                             */
/***************************************************************/

typedef struct PipeState_Entry_Struct{
  
  /* DE latches */
int DE_NPC,
    DE_IR,
    DE_V,
    /* AGEX lateches */
    AGEX_NPC,
    AGEX_SR1, 
    AGEX_SR2,
    AGEX_CC,
    AGEX_IR,
    AGEX_DRID,
    AGEX_V,
    AGEX_CS[NUM_AGEX_CS_BITS],
    /* MEM latches */
    MEM_NPC,
    MEM_ALU_RESULT,
    MEM_ADDRESS,
    MEM_CC,
    MEM_IR,
    MEM_DRID,
    MEM_V,
    MEM_CS[NUM_MEM_CS_BITS],
    /* SR latches */
    SR_NPC, 
    SR_DATA,
    SR_ALU_RESULT, 
    SR_ADDRESS,
    SR_IR,
    SR_DRID,
    SR_V,
    SR_CS[NUM_SR_CS_BITS];
    
} PipeState_Entry;

/* data structure for latch */
PipeState_Entry PS, NEW_PS;

/* simulator signal */
int RUN_BIT;

/* Internal stall signals */ 
int   dep_stall,
      v_de_br_stall,
      v_agex_br_stall,
      v_mem_br_stall,
      mem_stall,
      icache_r;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the architectural state    \n");
    printf("idump            -  dump the internal state         \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

void print_CS(int *CS, int num)
{
  int ii ;
  for ( ii = 0 ; ii < num; ii++) {
    printf("%d",CS[ii]);
  }
  printf("\n");
}
/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {
  NEW_PS = PS; 
  SR_stage();
  MEM_stage(); 
  AGEX_stage();
  DE_stage();
  FETCH_stage();
  PS = NEW_PS; 
  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
	if (PC == 0x0000) {
	    cycle();
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {
    if ((RUN_BIT == FALSE) || (PC == 0x0000)) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }
    printf("Simulating...\n\n");
    /* initialization */
    while (PC != 0x0000)
      cycle();
    cycle();
      RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a region of memory to the output file.     */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%04x..0x%04x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%04x..0x%04x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current architectural state  to the       */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k; 

    printf("\nCurrent architectural state :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count : %d\n", CYCLE_COUNT);
    printf("PC          : 0x%04x\n", PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    printf("Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
	printf("%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent architectural state :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC          : 0x%04x\n", PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : idump                                           */
/*                                                             */
/* Purpose   : Dump current internal state to the              */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void idump(FILE * dumpsim_file) {
    int k; 

    printf("\nCurrent architectural state :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count     : %d\n", CYCLE_COUNT);
    printf("PC              : 0x%04x\n", PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    printf("Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
	printf("%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    printf("\n");
    
    printf("------------- Stall Signals -------------\n");
    printf("ICACHE_R        :  %d\n", icache_r);
    printf("DEP_STALL       :  %d\n", dep_stall);
    printf("V_DE_BR_STALL   :  %d\n", v_de_br_stall);
    printf("V_AGEX_BR_STALL :  %d\n", v_agex_br_stall);
    printf("MEM_STALL       :  %d\n", mem_stall);
    printf("V_MEM_BR_STALL  :  %d\n", v_mem_br_stall);    
    printf("\n");

    printf("------------- DE   Latches --------------\n");
    printf("DE_NPC          :  0x%04x\n", PS.DE_NPC );
    printf("DE_IR           :  0x%04x\n", PS.DE_IR );
    printf("DE_V            :  %d\n", PS.DE_V);
    printf("\n");
    
    printf("------------- AGEX Latches --------------\n");
    printf("AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
    printf("AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
    printf("AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
    printf("AGEX_CC         :  %d\n", PS.AGEX_CC );
    printf("AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
    printf("AGEX_DRID       :  %d\n", PS.AGEX_DRID);
    printf("AGEX_CS         :  ");
    for ( k = 0 ; k < NUM_AGEX_CS_BITS; k++) {
      printf("%d",PS.AGEX_CS[k]);
    }
    printf("\n");
    printf("AGEX_V          :  %d\n", PS.AGEX_V);
    printf("\n");

    printf("------------- MEM  Latches --------------\n");
    printf("MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
    printf("MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
    printf("MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS ); 
    printf("MEM_CC          :  %d\n", PS.MEM_CC );
    printf("MEM_IR          :  0x%04x\n", PS.MEM_IR );
    printf("MEM_DRID        :  %d\n", PS.MEM_DRID);
    printf("MEM_CS          :  ");
    for ( k = 0 ; k < NUM_MEM_CS_BITS; k++) {
      printf("%d",PS.MEM_CS[k]);
    }
    printf("\n");
    printf("MEM_V           :  %d\n", PS.MEM_V);
    printf("\n");

    printf("------------- SR   Latches --------------\n");
    printf("SR_NPC          :  0x%04x\n", PS.SR_NPC );
    printf("SR_DATA         :  0x%04x\n", PS.SR_DATA );
    printf("SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
    printf("SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
    printf("SR_IR           :  0x%04x\n", PS.SR_IR );
    printf("SR_DRID         :  %d\n", PS.SR_DRID);
    printf("SR_CS           :  ");
    for ( k = 0 ; k < NUM_SR_CS_BITS; k++) {
      printf("%d",PS.SR_CS[k]);
    }
    printf("\n");
    printf("SR_V            :  %d\n", PS.SR_V);
    
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file,"\nCurrent architectural state :\n");
    fprintf(dumpsim_file,"-------------------------------------\n");
    fprintf(dumpsim_file,"Cycle Count     : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file,"PC              : 0x%04x\n", PC);
    fprintf(dumpsim_file,"CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    fprintf(dumpsim_file,"Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
      fprintf(dumpsim_file,"%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- Stall Signals -------------\n");
    fprintf(dumpsim_file,"ICACHE_R        :  %d\n", icache_r);
    fprintf(dumpsim_file,"DEP_STALL       :  %d\n", dep_stall);
    fprintf(dumpsim_file,"V_DE_BR_STALL   :  %d\n", v_de_br_stall);
    fprintf(dumpsim_file,"V_AGEX_BR_STALL :  %d\n", v_agex_br_stall);
    fprintf(dumpsim_file,"MEM_STALL       :  %d\n", mem_stall);
    fprintf(dumpsim_file,"V_MEM_BR_STALL  :  %d\n", v_mem_br_stall);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- DE   Latches --------------\n");
    fprintf(dumpsim_file,"DE_NPC          :  0x%04x\n", PS.DE_NPC );
    fprintf(dumpsim_file,"DE_IR           :  0x%04x\n", PS.DE_IR );
    fprintf(dumpsim_file,"DE_V            :  %d\n", PS.DE_V);
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- AGEX Latches --------------\n");
    fprintf(dumpsim_file,"AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
    fprintf(dumpsim_file,"AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
    fprintf(dumpsim_file,"AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
    fprintf(dumpsim_file,"AGEX_CC         :  %d\n", PS.AGEX_CC );
    fprintf(dumpsim_file,"AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
    fprintf(dumpsim_file,"AGEX_DRID       :  %d\n", PS.AGEX_DRID);
    fprintf(dumpsim_file,"AGEX_CS         :  ");
    for ( k = 0 ; k < NUM_AGEX_CS_BITS; k++) {
      fprintf(dumpsim_file,"%d",PS.AGEX_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"AGEX_V          :  %d\n", PS.AGEX_V);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- MEM  Latches --------------\n");
    fprintf(dumpsim_file,"MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
    fprintf(dumpsim_file,"MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
    fprintf(dumpsim_file,"MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS ); 
    fprintf(dumpsim_file,"MEM_CC          :  %d\n", PS.MEM_CC );
    fprintf(dumpsim_file,"MEM_IR          :  0x%04x\n", PS.MEM_IR );
    fprintf(dumpsim_file,"MEM_DRID        :  %d\n", PS.MEM_DRID);
    fprintf(dumpsim_file,"MEM_CS          :  ");
    for ( k = 0 ; k < NUM_MEM_CS_BITS; k++) {
      fprintf(dumpsim_file,"%d",PS.MEM_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"MEM_V           :  %d\n", PS.MEM_V);
    fprintf(dumpsim_file,"\n");

    fprintf(dumpsim_file,"------------- SR   Latches --------------\n");
    fprintf(dumpsim_file,"SR_NPC          :  0x%04x\n", PS.SR_NPC );
    fprintf(dumpsim_file,"SR_DATA         :  0x%04x\n",PS.SR_DATA );
    fprintf(dumpsim_file,"SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
    fprintf(dumpsim_file,"SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
    fprintf(dumpsim_file,"SR_IR           :  0x%04x\n", PS.SR_IR );
    fprintf(dumpsim_file,"SR_DRID         :  %d\n", PS.SR_DRID);
    fprintf(dumpsim_file,"SR_CS           :  ");
    for ( k = 0 ; k < NUM_SR_CS_BITS; k++) {
    fprintf(dumpsim_file, "%d",PS.SR_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"SR_V            :  %d\n", PS.SR_V);
    
    fprintf(dumpsim_file,"\n");
    
    
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

    case 'I':
    case 'i':
        idump(dumpsim_file);
        break;
	
    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < NUM_CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}
	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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


/***************************************************************/
/*                                                             */
/* Procedure : init_state                                      */
/*                                                             */
/* Purpose   : Zero out all latches and registers              */
/*                                                             */
/***************************************************************/
void init_state() {
  
  memset(&PS, 0 ,sizeof(PipeState_Entry)); 
  memset(&NEW_PS, 0 , sizeof(PipeState_Entry));
  
  dep_stall       = 0; 
  v_de_br_stall   = 0;
  v_agex_br_stall = 0;
  v_mem_br_stall  = 0;
  mem_stall       = 0;
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
	program_base = word >> 1 ;
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

    if (PC == 0) PC  = program_base << 1 ;
    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();

    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(program_filename);
	while(*program_filename++ != '\0');
    }
    init_state();
    
    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* dcache_access                                               */
/*                                                             */
/***************************************************************/
void dcache_access(int dcache_addr, int *read_word, int write_word, int *dcache_r, 
		    int mem_w0, int mem_w1) {
  
  int addr = dcache_addr >> 1 ; 
  int random = CYCLE_COUNT % 9;

  if (!random) {
    *dcache_r = 0;
    *read_word = 0xfeed ;
  }
  else {
    *dcache_r = 1;
    
    *read_word = (MEMORY[addr][1] << 8) | (MEMORY[addr][0] & 0x00FF);
    if(mem_w0) MEMORY[addr][0] = write_word & 0x00FF;
    if(mem_w1) MEMORY[addr][1] = (write_word & 0xFF00) >> 8;
  }
}
/***************************************************************/
/*                                                             */
/* icache_access                                               */
/*                                                             */
/***************************************************************/
void icache_access(int icache_addr, int *read_word, int *icache_r) {
	
  int addr = icache_addr >> 1 ; 
  int random = CYCLE_COUNT % 13;

  if (!random) {
    *icache_r = 0;
    *read_word = 0xfeed;
  }
  else {
    *icache_r = 1;
    *read_word = MEMORY[addr][1] << 8 | MEMORY[addr][0];
  }
}
/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

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
   
   RUN_BIT
   REGS
   MEMORY

   PC
   N
   Z
   P

   dep_stall
   v_de_br_stall
   v_agex_br_stall
   v_mem_br_stall
   mem_stall 
   icache_r
 
   PS
   NEW_PS


   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.


   Begin your code here 	  			       */
/***************************************************************/
#define COPY_AGEX_CS_START 3 
#define COPY_MEM_CS_START 9
#define COPY_SR_CS_START  7

#define DEBUG_F 1
#define DEBUG_DE 0
#define DEBUG_AGEX 0
#define DEBUG_MEM 0
#define DEBUG_SR 0

/* Signals generated by SR stage and needed by previous stages in the
   pipeline are declared below. */
int sr_reg_data, 
    sr_n, sr_z, sr_p,
    v_sr_ld_cc,
    v_sr_ld_reg,
    sr_reg_id;

int mem_pcmux, mem_stall, target_pc, trap_pc;

int v_agex_ld_cc;
int v_agex_ld_reg;

int v_mem_ld_cc;
int v_mem_ld_reg;




int out_icache;

/************************* SR_stage() *************************/
void SR_stage() {
  
  /* You are given the code for SR_stage to get you started. Look at
     the figure for SR stage to see how this code is implemented. */
  
  switch (Get_DR_VALUEMUX1(PS.SR_CS))
  {
  case 0: 
    sr_reg_data = PS.SR_ADDRESS ;
    break;
  case 1:
    sr_reg_data = PS.SR_DATA ;
    break;
  case 2:
    sr_reg_data = PS.SR_NPC ;
    break;
  case 3:
    sr_reg_data = PS.SR_ALU_RESULT ;
    break;
  }

  sr_reg_id = PS.SR_DRID; 
  v_sr_ld_reg = Get_SR_LD_REG(PS.SR_CS) & PS.SR_V;
  v_sr_ld_cc = Get_SR_LD_CC(PS.SR_CS) & PS.SR_V ;

  /* CC LOGIC  */
  sr_n = ((sr_reg_data & 0x8000) ? 1 : 0);
  sr_z = ((sr_reg_data & 0xFFFF) ? 0 : 1);
  sr_p = 0;
  if ((!sr_n) && (!sr_z))
    sr_p = 1;
  
}


/************************* MEM_stage() *************************/
void MEM_stage() {

/*
*/

  int ii,jj = 0;
	int dcache_r;
  int out_dcache;
	int v_dcache_en;
	int in_we1, in_we0;
	int in_dcache_data;
  /* your code for MEM stage goes here */

  
  /* br logic*/
	if(Get_TRAP_OP(PS.MEM_CS))
	{
		if(PS.MEM_V)
			mem_pcmux = 2;
		else
			mem_pcmux = 0;
	}
	else if(Get_BR_OP(PS.MEM_CS))
	{
		if(PS.MEM_V)
			mem_pcmux = 0;
		else
			mem_pcmux = 0;
	}
	else if(Get_UNCOND_OP(PS.MEM_CS))
	{
		if(PS.MEM_V)
			mem_pcmux = 1;
		else
			mem_pcmux = 0;
	}
	else
	{
		mem_pcmux = 0;
	}
  
  
  /* LATCH SR.ADDRESS */
  NEW_PS.SR_ADDRESS = PS.MEM_ADDRESS;
	target_pc = PS.MEM_ADDRESS;
	
	/* LATCH SR.DATA */
	v_dcache_en = Get_DCACHE_EN(PS.MEM_CS) && PS.MEM_V;
	if(v_dcache_en)
	{
		if(Get_DCACHE_RW(PS.MEM_CS))
		{
			/*WE LOGIC*/
			/*write*/
			if(PS.MEM_ADDRESS & 0x0001)
			{
			/* odd */
				if(Get_DATA_SIZE(PS.MEM_CS))
				{
					/*WORD*/
					printf("****************************INVALID odd address word**************************\n");
					in_we0 = 0;
					in_we1 = 0;
				}
				else
				{
				/*byte*/
					in_we0 = 0;
					in_we1 = 1;
				}
			}
			else
			{
			/*even*/
				if(Get_DATA_SIZE(PS.MEM_CS))
				{
					/*WORD*/
					in_we0 = 1;
					in_we1 = 1;
				}
				else
				{
				/*byte*/
					in_we0 = 1;
					in_we1 = 0;
				}
			}
			/*write data*/
		}
		else
		{
		/*read*/
			in_we0 = 0;
			in_we1 = 0;
		}
		
		/*STB & STW LOGIC*/
		if(Get_DATA_SIZE(PS.MEM_CS))
		{
			/* word */
			in_dcache_data = PS.MEM_ALU_RESULT;	
		}
		else
		{
			/* byte */
			in_dcache_data = (PS.MEM_ALU_RESULT & 0x00FF) + ((PS.MEM_ALU_RESULT << 8) & 0xFF00);
		}

		
		dcache_access(PS.MEM_ADDRESS, &out_dcache, in_dcache_data, &dcache_r, 
		    in_we0, in_we1);
		if(DEBUG_MEM)
			{
				printf("********************\n");
				printf("MEM STAGE\n");
				printf("we1: %d we0: %d \n", in_we1, in_we0);
				printf("in_dcache: %x \n", in_dcache_data	);
				printf("out_dcache: %x \n", out_dcache);
				printf("mem en: %d \n", v_dcache_en);
				printf("****************\n");
		}				
		if(Get_DATA_SIZE(PS.MEM_CS))
		{
		/*ldw*/
			NEW_PS.SR_DATA = out_dcache;
		}
		else
		{
			/*ldb*/
			if(PS.MEM_ADDRESS & 0x0001)
			{
				/*odd */																						
				if((out_dcache >> 15) & 0x0001)
				{
					/*sext*/
					NEW_PS.SR_DATA = (out_dcache >> 8) | 0xFF00;
				}
				else
				{
					/*zext*/
					NEW_PS.SR_DATA = (out_dcache >> 8) & 0x00FF;
				}
			}
			else
			{
				/*even */																											
				if((out_dcache >> 7) & 0x0001)
				{
					/*sext*/
					NEW_PS.SR_DATA = (out_dcache) | 0xFF00;
				}
				else
				{
					/*zext*/
					NEW_PS.SR_DATA = (out_dcache) & 0x00FF;
				}
			}			
		}
	}
	else
	{
		NEW_PS.SR_DATA = 0;																														/************************************MAY HAVE TO CHANGE********************************************/
	}
	trap_pc = NEW_PS.SR_DATA;
	if(!dcache_r && v_dcache_en)
	{
		mem_stall = 1;
	}
	else
	{
		mem_stall = 0;
	}
	
	
  
  /* The code below propagates the control signals from MEM.CS latch
     to SR.CS latch. You still need to latch other values into the
     other SR latches. */
  for (ii=COPY_SR_CS_START; ii < NUM_MEM_CS_BITS; ii++) {
    NEW_PS.SR_CS [jj++] = PS.MEM_CS [ii];
  }
  
  /* LATCH SR.NPC */
  NEW_PS.SR_NPC = PS.MEM_NPC;
  
  /* LATCH SR.ALU_RESULT */
  NEW_PS.SR_ALU_RESULT = PS.MEM_ALU_RESULT;
  
  /* LATCH SR.IR */
  NEW_PS.SR_IR = PS.MEM_IR;
  
  /* LATCH SR.DRID */
  NEW_PS.SR_DRID = PS.MEM_DRID;
  
  /* LATCH SR.V */
  NEW_PS.SR_V = PS.MEM_V && !(mem_stall);																													/*??????????????HAS TO BE CHANGED?????????????????????????*/

	
		/* logic for v_mem_br_stall */
	if(PS.MEM_V)
	{
		if(Get_MEM_BR_STALL(PS.MEM_CS))
		{
			v_mem_br_stall = 1;
		}
		if(Get_MEM_LD_CC(PS.MEM_CS))
		{
			v_mem_ld_cc = 1;
		}
		if(Get_MEM_LD_REG(PS.MEM_CS))
		{
			v_mem_ld_reg = 1;
		}
	}
	else
	{
		v_mem_ld_cc = 0;
		v_mem_ld_reg = 0;
		v_mem_br_stall = 0;	
	}
}


/************************* AGEX_stage() *************************/
void AGEX_stage() {

  int ii, jj = 0;
  int LD_MEM = 1; /* You need to write code to compute the value of LD.MEM														/*??????????????HAS TO BE CHANGED?????????????????????????
		 signal */

  /* your code for AGEX stage goes here */
  int OUT_SR2MUX;
	int OUT_ALU;
	int OUT_SHF;

  
  
 
  if (LD_MEM) {
    /* Your code for latching into MEM latches goes here */
   
   

  
  if(Get_SR2MUX(PS.AGEX_CS))
  {
		if((PS.AGEX_IR >> 4)& 0x0001)
		{
			OUT_SR2MUX = PS.AGEX_IR | 0xFFE0;
		}
		else
		{
			OUT_SR2MUX = PS.AGEX_IR & 0x001F;
		}
  }
  else
  {
	OUT_SR2MUX = PS.AGEX_SR2;
  }
  
  switch(Get_ALUK(PS.AGEX_CS))
  {
		case 0: /* ADD */
		{
			OUT_ALU = Low16bits(PS.AGEX_SR1 + OUT_SR2MUX);
			break;
		}
		case 1: /* AND */
		{
			OUT_ALU = Low16bits(PS.AGEX_SR1 & OUT_SR2MUX);
			break;
		}
		case 2: /* XOR */
		{
			OUT_ALU = Low16bits(PS.AGEX_SR1 ^ OUT_SR2MUX);
			break;
		}
		case 3: /* PASS B*/
		{
			OUT_ALU = Low16bits(OUT_SR2MUX);
			break;
		}
  }
	
	if((PS.AGEX_IR >> 12) == 13)
	{
		/*SHF*/
		if(((PS.AGEX_IR >> 4) & 0x0001) == 0)
		{
			OUT_SHF = Low16bits(PS.AGEX_SR1 << (PS.AGEX_IR & 0x000F));
		}
		else
		{
			if(((PS.AGEX_IR >> 5) & 0x0001) == 0)
			{
				OUT_SHF = Low16bits(PS.AGEX_SR1 >> (PS.AGEX_IR & 0x000F)) & 0x7FFF;
			}
			else
			{
				if((PS.AGEX_SR1 >> 15) & 0x0001) /* if negative then sext */
				{
					OUT_SHF = Low16bits((PS.AGEX_SR1 | 0xFFFF0000) >> (PS.AGEX_IR & 0x000F));
				}
				else
				{
					OUT_SHF = Low16bits(PS.AGEX_SR1 >> (PS.AGEX_IR & 0x000F));
				}
			}
		}
	}
	else
	{
		OUT_SHF = 0;
	}
  
  
  /* latch mem.address */
	if(Get_ADDRESSMUX(PS.AGEX_CS))
	{
		int out_addr2mux, out_addr1mux;
		if(Get_ADDR1MUX(PS.AGEX_CS))
		{
			out_addr1mux = PS.AGEX_SR1;
		}
		else
		{
			out_addr1mux = PS.AGEX_NPC;
		}
		switch(Get_ADDR2MUX(PS.AGEX_CS))
		{
			case 0:
			{
				out_addr2mux = 0;
				break;
			}
			case 1:
			{
				if((PS.AGEX_IR >> 5) & 0x0001)
				{
					out_addr2mux = PS.AGEX_IR | 0xFFC0;
				}
				else
				{
					out_addr2mux = PS.AGEX_IR & 0x003F;
				}
				break;
			}
			case 2:
			{
				if((PS.AGEX_IR >> 8) & 0x0001)
				{
					out_addr2mux = PS.AGEX_IR | 0xFE00;
				}
				else
				{
					out_addr2mux = PS.AGEX_IR & 0x01FF;
				}
				break;
			}
			case 3:
			{
				if((PS.AGEX_IR >> 10) & 0x0001)
				{
					out_addr2mux = PS.AGEX_IR | 0xF800;
				}
				else
				{
					out_addr2mux = PS.AGEX_IR & 0x07FF;
				}
				break;
			}
		}
		if(Get_LSHF1(PS.AGEX_CS))
		{
			out_addr2mux = out_addr2mux << 1;
		}
		
	  if(DEBUG_AGEX)
		{
			printf("********************\n");
			printf("AGEX STAGE\n");
			printf("out_addr1mux: %x out addr2mux: %x\n", out_addr1mux, out_addr2mux);

			printf("****************\n");
		}
		NEW_PS.MEM_ADDRESS = Low16bits(out_addr1mux + out_addr2mux);
	}
	else
	{
	/* addressmux = 0 */
		NEW_PS.MEM_ADDRESS = (PS.AGEX_IR & 0x00FF) << 1; /*lshf(zext[7:0], 1)*/
	}
  

    /* The code below propagates the control signals from AGEX.CS latch
       to MEM.CS latch. */
    for (ii = COPY_MEM_CS_START; ii < NUM_AGEX_CS_BITS; ii++) {
      NEW_PS.MEM_CS [jj++] = PS.AGEX_CS [ii]; 
    }
	
	/* latch mem.npc */
	NEW_PS.MEM_NPC = PS.AGEX_NPC;
	
	/* LATCH MEM.CC */
	NEW_PS.MEM_CC = PS.AGEX_CC;
	
	/* LATCH MEM.ALU.RESULT*/
	if(Get_ALU_RESULTMUX(PS.AGEX_CS))
	{
		NEW_PS.MEM_ALU_RESULT = OUT_ALU;
	}
	else
	{
		NEW_PS.MEM_ALU_RESULT = OUT_SHF;
	}
	
	/* LATCH MEM.IR */
	NEW_PS.MEM_IR = PS.AGEX_IR;
	
	/* LATCH MEM.DRID */
	NEW_PS.MEM_DRID = PS.AGEX_DRID;
	
	/* LATCH MEM.V */
	NEW_PS.MEM_V = PS.AGEX_V;																											/*??????????????HAS TO BE CHANGED?????????????????????????*/
	
  }
	
	
	/* logic for v_agex_br_stall */
	if(PS.AGEX_V)
	{
		if(Get_AGEX_BR_STALL(PS.AGEX_CS))
		{
			v_agex_br_stall = 1;
		}
		if(Get_AGEX_LD_CC(PS.AGEX_CS))
		{
			v_agex_ld_cc = 1;
		}
		if(Get_AGEX_LD_REG(PS.AGEX_CS))
		{
			v_agex_ld_reg = 1;
		}
	}
	else
	{
		v_agex_ld_cc = 0;
		v_agex_ld_reg = 0;
		v_agex_br_stall = 0;	
	}
}



/************************* DE_stage() *************************/
void DE_stage() {



  int CONTROL_STORE_ADDRESS = ((PS.DE_IR >> 10) & 0x003E) + ((PS.DE_IR >> 5) & 0x0001);  /* You need to implement the logic to
			         set the value of this variable. Look
			         at the figure for DE stage */ 
  int ii, jj = 0;
  int LD_AGEX = 1; /* You need to write code to compute the value of 														/*??????????????HAS TO BE CHANGED?????????????????????????
		  LD.AGEX signal */

  /* your code for DE stage goes here */
	
	int in_sr1_needed;
	int in_sr2_needed;
	int in_drmux;
	int in_de_br_op;
	int in_de_br_stall;
	int in_sr1, in_sr2;
	
  if(DEBUG_DE)
  {
	printf("********************\n");
	printf("DE STAGE\n");
	printf("v_sr_ld_cc: %d\n", v_sr_ld_cc);
	printf("sr_cc %d%d%d\n", sr_n, sr_z, sr_p);
	printf("LD_AGEX: %d\n", LD_AGEX);
	printf("DE_IR: %x\n", PS.DE_IR);
	printf("CONTROL_STORE_ADDRESS: %d\n", CONTROL_STORE_ADDRESS);
	printf("****************\n");
  }
  



  if (LD_AGEX) {
    /* Your code for latching into AGEX latches goes here */
	
  if(DEBUG_DE)
  {
		printf("********************\n");
		printf("DE STAGE\n");
		printf("LOADING AGEX...\n");
		printf("****************\n");
  }
	
	NEW_PS.AGEX_NPC = PS.DE_NPC; /* latch npc */

    /* The code below propagates the control signals from the CONTROL
       STORE to the AGEX.CS latch. */
			 
	for (ii = COPY_AGEX_CS_START; ii< NUM_CONTROL_STORE_BITS; ii++) {
    NEW_PS.AGEX_CS[jj++] = CONTROL_STORE[CONTROL_STORE_ADDRESS][ii];
	}
		
		
	in_sr1_needed = CONTROL_STORE[CONTROL_STORE_ADDRESS][0];
	in_sr2_needed = CONTROL_STORE[CONTROL_STORE_ADDRESS][1];
	in_drmux = CONTROL_STORE[CONTROL_STORE_ADDRESS][2];
	in_de_br_op =  CONTROL_STORE[CONTROL_STORE_ADDRESS][12];
	in_de_br_stall =  CONTROL_STORE[CONTROL_STORE_ADDRESS][15];
	
	if(in_de_br_stall)
	{
		if(PS.DE_V)
		{
			v_de_br_stall = 1;
		}
		else
		{
			v_de_br_stall = 0;
		}
	}
	else
	{
		v_de_br_stall = 0;
	}
	

	
	NEW_PS.AGEX_IR = PS.DE_IR; /* latch ir */
	
	if(in_sr1_needed)
	{
		in_sr1 = (PS.DE_IR >> 6) & 0x0007;
	}
	else
	{
		in_sr1 = 0;				/* awwwwwwwwwwwwwwwwwwwwww yeaaaaaaaa*/
	}
	
	if(in_sr2_needed)
	{
		if((PS.DE_IR >> 13) & 0x0001)
		{
			in_sr2 = (PS.DE_IR >> 9) & 0x0007; /* sr2.idmux = 1 */
		}
		else
		{
			in_sr2 = PS.DE_IR & 0x0007; /* sr2.idmux = 0 */
		}
	}
	else
	{
		in_sr2 = 0;			/* awwwwwwwwwwwwwwwwwwwwww yeaaaaaaaa*/
	}
	
	/*~~~~~~~~DEPENDENCY LOGIC~~~~~~~*/
	if(PS.DE_V)
	{
		if(in_sr2_needed)
		{
			if(((in_sr1 == PS.AGEX_DRID) && v_agex_ld_reg)
			|| ((in_sr1 == PS.MEM_DRID) && v_mem_ld_reg)
			|| ((in_sr1 == PS.SR_DRID) && v_sr_ld_reg)
			|| ((in_sr2 == PS.AGEX_DRID) && v_agex_ld_reg)
			|| ((in_sr2 == PS.MEM_DRID) && v_mem_ld_reg)
			|| ((in_sr2 == PS.SR_DRID) && v_sr_ld_reg)
			)
			{
				dep_stall = 1;
			}
			else
			{
				dep_stall = 0;
			}
		}
		else if(in_sr2_needed == 0)
		{
			if(((in_sr1 == PS.AGEX_DRID) && v_agex_ld_reg)
			|| ((in_sr1 == PS.MEM_DRID) && v_mem_ld_reg)
			|| ((in_sr1 == PS.SR_DRID) && v_sr_ld_reg)
			)
			{
				dep_stall = 1;
			}
			else
			{
				dep_stall = 0;
			}
		}
		else
		{
			dep_stall = 0;
		}
	}
	else
	{
		dep_stall = 0;
	}
	
	if(DEBUG_DE)
  {
		printf("********************\n");
		printf("DE STAGE DEP CHECK\n");
		printf("DE V: %d\n", PS.DE_V);
		printf("in sr1: %d\n", in_sr1);
		printf("in sr2: %d\n", in_sr2);
		printf("in sr1 needed: %d\n", in_sr1_needed);
		printf("in sr2 needed: %d\n", in_sr2_needed);
		printf("agex drid: %d\n", PS.AGEX_DRID);
		printf("MEM drid: %d\n", PS.MEM_DRID);
		printf("sr drid: %d\n", PS.SR_DRID);
		printf("dep_stall: %d\n", dep_stall);
		printf("****************\n");
  }
	
	NEW_PS.AGEX_SR1 = REGS[in_sr1]; /* latch sr1 */
	
	NEW_PS.AGEX_SR2 = REGS[in_sr2]; /* latch sr2 */	
	/*
		sr_reg_data, 
    sr_n, sr_z, sr_p,
    v_sr_ld_cc,
    v_sr_ld_reg,
    sr_reg_id;

	*/	
	/*latch reg values*/
	if(v_sr_ld_reg)
	{
		REGS[sr_reg_id] = sr_reg_data;
	}
	
	/* latch cc */
	if(v_sr_ld_cc)
	{
		N = sr_n;
		Z = sr_z;
		P = sr_p;
	}
	else
	{
		NEW_PS.AGEX_CC = (N << 2) + (Z << 1) + P;
	}
	
	/* LATCH AGEX.DRID */
	if(in_drmux)
	{
		NEW_PS.AGEX_DRID = 7;
	}
	else
	{
		NEW_PS.AGEX_DRID = (PS.DE_IR >> 9) & 0x0007;
	}
	
	/* latch agexv */
	NEW_PS.AGEX_V = PS.DE_V && (!dep_stall); 																							/*!!!!!!!!!!!!!!!!!!!!!!!!!!!HAS TO BE CHANGED!!!!!!!!!!!!!!!!!!!!!!!!*/
	
	
	if(DEBUG_DE)
  {
		printf("********************\n");
		printf("FINISH LOADING AGEX\n");
		printf("****************\n");
  }
	
  }

}



/************************* FETCH_stage() *************************/
void FETCH_stage() {

  /* your code for FETCH stage goes here 
	dep_stall
	v_de_br_stall
	v_agex_br_stall
	v_mem_br_stall
	mem_stall 
	icache_r
	void icache_access(int icache_addr, int *read_word, int *icache_r)
  */
  int LD_DE = !(mem_stall || dep_stall);
  int LD_PC; /**/

	icache_access(PC, &out_icache, &icache_r);
	LD_PC = !(dep_stall || mem_stall ||  v_de_br_stall || v_agex_br_stall) &&(icache_r);
  if(DEBUG_F)
  {
		printf("********************\n");
		printf("F STAGE\n");
		printf("current pc: 0x%x\n", PC);
		printf("mem_pcmux: %d\n", mem_pcmux);
		printf("Ld.PC %d\n", LD_PC);
		printf("****************\n");
  }  
  
  if(LD_DE)
  {
		if(DEBUG_F)
		{
			printf("********************\n");
			printf("F STAGE ld.de\n");
			printf("out_icache: %x\n", out_icache);
			printf("icache_r: %d\n", icache_r);
			printf("trap_pc: %x\n", trap_pc);
			printf("****************\n");
		}
		NEW_PS.DE_NPC = PC + 2;
		NEW_PS.DE_IR = out_icache;
		NEW_PS.DE_V = icache_r && !(v_de_br_stall || v_agex_br_stall || v_mem_br_stall);				/*!(v_de_br_stall || v_agex_br_stall || v_mem_br_stall || ~(icache_r));*/
		

  }
	if(LD_PC)
	{
		switch(mem_pcmux)
		{
			case 1:
			{
				PC = target_pc;
				break;
			}
			case 2:
			{
				PC = trap_pc;
				break;
			}
			default:
			{
				PC = Low16bits(PC + 2);
				break;
			}
		}
		if(DEBUG_F)
			printf("loaded pc: 0x%x\n", PC);
	}
}  
