/*
	Name 1: Duc Tran
	Name 2: Brandon Wong 
	UTEID 1: dmt735
	UTEID 2: blw868
*/
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define DEBUG_EN0 0
#define DEBUG_EN 1
#define DEBUG_EN1 0
#define  ADD 4096
#define  AND 20480
#define  BRN 2048
#define  BRP 512
#define  BRNP 2560
#define  BR 3584
#define  BRZ 1024
#define  BRNZ 3072
#define  BRZP 1536
#define  BRNZP 3584
#define  JMP 49152 
#define  JSR 18432
#define  JSRR 16384
#define  LDB 8192
#define  LDW 24576
#define  LEA 57344
#define  NOT 36927
#define  RET 49600
#define  RTI 32768
#define  LSHF 53248
#define  RSHFL 53264
#define  RSHFA 53296
#define  STB 12288
#define  STW 28672
#define  TRAP 61440
#define  XOR 36864
#define  HALT 61477
FILE* infile = NULL;
FILE* outfile = NULL;
typedef struct {
	int address;
	char label[MAX_LABEL_LEN +1];
}TableEntry;

TableEntry symbolTable[MAX_SYMBOLS];



enum{
	 DONE, OK, EMPTY_LINE
};


/***************list of opcode********************/

char* opcodeName[32] =
{
	"add",
	"and",
	"brn",
	"brp", 
	"brnp",
	"br",
	"brz",
	"brnz",
	"brzp",
	"brnzp",
	"jmp",
	"jsr",
	"jsrr",
	"ldb",
	"ldw",
	"lea",
	"not",
	"ret",
	"rti",
	"lshf",
	"rshfl",
	"rshfa",
	"stb",
	"stw",
	"trap",
	"xor", 
	"halt",
	"nop",
    "nop",
    "nop",
    "nop",
    "nop"  /**/
};
char *pseudoOpcode[3]=
{
    ".orig",
    ".fill",
    ".end"
};

/*

**************isOpcode*************
input: char pointer to string
output:
	-1 : not valid opcode
	OK : valid opcode
	
*/

int isOpcode(char * inString){
	int i;
	for(i=0; i < 31;i++){
		if(strcmp(opcodeName[i],inString)==0){
			if(DEBUG_EN0)
				printf("opcode found: %s %s \n", inString,opcodeName[i]);
			return(OK);
            
		}
	}
	if(DEBUG_EN0)
		printf("Not an opcode %s \n",inString);
	return(-1);
}

/*************End isOpcode************/
/*

**************isPseudoOpcode*************
input: char pointer to string
output:
	-1 : not valid opcode
	OK : valid opcode
	
*/

int isPseudoOpcode(char * inString){
	int i;
	for(i=0; i < 3;i++){
		if(strcmp(pseudoOpcode[i],inString)==0){
			if(DEBUG_EN0)
				printf("pseudo opcode found: %s %s \n", inString,pseudoOpcode[i]);
			return(OK);
            
		}
	}
	if(DEBUG_EN0)
		printf("Not an pseudo opcode %s \n",inString);
	return(-1);
}

/*************End isPseudoOpcode************/

/***************regToNum************/
int regToNum(char *reg, int argno){
	if(strcasecmp(reg, "R0")==0)
	{
		switch(argno)
		{
			case 1: return 0;
			case 2: return 0;
			case 3: return 0;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R1")==0)
	{
		switch(argno)
		{
			case 1: return 512;
			case 2: return 64;
			case 3: return 1;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R2")==0)
	{
		switch(argno)
		{
			case 1: return 1024;
			case 2: return 128;
			case 3: return 2;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R3")==0)
	{
		switch(argno)
		{
			case 1: return 1536;
			case 2: return 192;
			case 3: return 3;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R4")==0)
    
	{
		switch(argno)
		{
			case 1: return 2048;
			case 2: return 256;
			case 3: return 4;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R5")==0)
	{
		switch(argno)
		{
			case 1: return 2560;
			case 2: return 320;
			case 3: return 5;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R6")==0)
	{
		switch(argno)
		{
			case 1: return 3072;
			case 2: return 384;
			case 3: return 6;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else if(strcasecmp(reg, "R7")==0)
	{
		switch(argno)
		{
			case 1: return 3584;
			case 2: return 448;
			case 3: return 7;
			default: printf("**INVALID ARG**\n");
					return 0;
		}
	}
	else
	{
		printf("**ERROR 4 INVALID REGISTER**\n");
		exit(4);
	}
}


/*********toNum**************/
int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error 4: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error 4: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error 4: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}
/***********END toNum******************/


/************convertBR*********/
/*Return offset value to be add to opecode*/
int convertBR(int labellen,int memaddr, char *arg1 , char *arg2, char*arg3){
	int i;
	int offset=0;
	
	if(strcmp(arg2, "\0") !=0 || strcmp(arg3,"\0")!=0)
	{
		printf("ERROR 4: Unwanted Field!");
		exit(4);
	}
	if(strcmp(arg1,"\0") == 0){
		printf("ERROR 4: Missing Field");
		exit(4);}
	else
	{
		if(arg1[0] == 'x' || arg1[0] == '#'){
			printf("ERROR 4: not a valid label");
			exit(4);
		}
		for (i=0;i<labellen;i++)
		{
			if(strcmp(symbolTable[i].label,arg1) ==0)
			{
				offset = (symbolTable[i].address-memaddr)/2;
				if(offset >= -256 && offset <= 255)
				{
					if(offset < 0)
					{
						offset = offset - 0xFFFFFE00;
					}
					return(offset);
				}
				else
				{
					printf("ERROR 3: OFFSET TOO LARGE\n");
					exit(3);
				}
				
			}	
		}
		printf("ERROR 1: Undefined label!!!");
		exit(1); 
	}
	

}

/*********EndConvertBR********/



/***********readAndParse*************/

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	)
	{
	   char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
       /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		if(isdigit(lPtr[0]) != 0 || tolower(lPtr[0]) == 'x' || isalnum(lPtr[0]) == 0) 
		{
			printf("**ERROR 4 INVALID LABEL**\n");
			exit(4);
			
		}
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   
           *pOpcode = lPtr;
	
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr;
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
	}




/**************End readAndParse*******/

/************assembleCommand*************/
/*
input:
   int value of Lable table lenght
output:
    DONE
Error code:
    exit(n)
    1- undefined label
    2- invalid opcode
    3- invalid constant
    4- other error
*/
int assembleCommand(int symbolTableLen){
    int lRet,i;
    int memAddr =0;
    int opcode, arg1, arg2, arg3;
	int retVal;
	int labelFlag;
    /*Line parsing init*/
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	        *lArg2, *lArg3, *lArg4;
     i =0;
    do
    {
        lRet = readAndParse( infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
            memAddr++;
			memAddr++;
			retVal = 0;
            /********For Debug*********/
            if(DEBUG_EN1){
                i++;
                printf("*****Line: %d ***** \n", i);
                printf("Label: %s \n",lLabel);
                printf("Opcode: %s \n",lOpcode); 
                printf("Arg1: %s \n",lArg1);
                printf("Arg2: %s \n",lArg2);
                printf("Arg3: %s \n",lArg3);
                printf("Arg4: %s \n",lArg4);
            }
            /********EndDebugBlock*****/

            /***Do computation here****/
            if(isOpcode(lOpcode) == -1 && isPseudoOpcode(lOpcode) == -1)
			{
				printf("**ERROR 2 INVALID OPCODE**");
                exit(2); /*Opcode not define in ISA*/
			}
            if(strcmp(lOpcode,".orig")==0){
                memAddr = toNum(lArg1);
				fprintf(outfile, "0x%0.4X\n", toNum(lArg1));
            }
			else if(strcmp(lOpcode,".fill") == 0){
				if(strcmp(lArg1,"\0")==0 || strcmp(lArg2,"\0") !=0){
					printf("Error 4: Unwanted field or Missing field");
					exit(4);
				}
				retVal = (toNum(lArg1));
				if(retVal > 0xFFFF || retVal < -32768){
					printf("ERROR 3: INVALID CONSTANT IN FILL");
					exit(3);
				}
				if(retVal <=0)
				{
					retVal = retVal - 0xFFFF0000;
				}
				fprintf(outfile,"0x%0.4X\n",retVal);
			}
			else if(strcmp(lOpcode,".end") == 0){
				if(strcmp(lArg1,"\0")!=0)
				{
					printf("ERROR 4: Unwanted Field \n");
					exit(4);
				}
				lRet = DONE;
			}
            else if(strcmp(lOpcode,"nop") ==0){
                if(strcmp(lArg1,"\0")!=0){
                    printf("Error!!! Unwanted Field \n");
                    exit(4);
                    }
                fprintf(outfile,"0x%0.4X\n",0); /*NOP 0X0000*/
            }
            else if(strcmp(lOpcode,"and" )==0){
                              
                if(strcmp(lArg1,"\0")==0){
                    printf("Error!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    arg1 = regToNum(lArg1, 1);
                }
                if(strcmp(lArg2,"\0")==0){
                    printf("Error!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    arg2 = regToNum(lArg2, 2);
                }
                
                if(strcmp(lArg3,"\0")==0){
                    printf("Error!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    if(lArg3[0] == 'r')
                    {
                        arg3 = regToNum(lArg3, 3);
                    }
                    else if(lArg3[0] == '#' || lArg3[0] == 'x')
                    {
                        if(toNum(lArg3)>=-16 && toNum(lArg3)<=15)
                        {
							retVal = toNum(lArg3);
							if(retVal <0)
							{
								retVal = retVal - 0xFFFFFFE0;
							}
                            arg3 = toNum(lArg3) + 32;
                        }
                        else
                        {
                            printf("**ERROR 3 Invalid Constant**\n");
                            exit(3);
                        }
                    }
                    else
                    {
                        printf("**ERROR 4 INVALID ARG**\n");
                        exit(4);
                    }	
                }
                fprintf(outfile,"0x%0.4x\n", AND + arg1 + arg2 + arg3);
            }
            else if(strcmp(lOpcode,"add" )==0){
               
                if(strcmp(lArg1,"\0")==0){
                    printf("Error4!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    arg1 = regToNum(lArg1, 1);
                }
                if(strcmp(lArg2,"\0")==0){
                    printf("Error4!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    arg2 = regToNum(lArg2, 2);
                }
                
                if(strcmp(lArg3,"\0")==0){
                    printf("Error 4!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    if(lArg3[0] == 'r')
                    {
                        arg3 = regToNum(lArg3, 3);
                    }
                    else if(lArg3[0] == '#' || lArg3[0] == 'x')
                    {
                        if(toNum(lArg3)>=-16 && toNum(lArg3)<=15)
                        {
							retVal = toNum(lArg3);
							if(retVal <0)
							{
								retVal = retVal - 0xFFFFFFE0;
							}
                            arg3 = retVal + 32;
                        }
                        else
                        {
                            printf("**ERROR 3 Invalid Constant**\n");
                            exit(3);
                        }
                    }
                    else
                    {
                        printf("**ERROR 4 INVALID ARG*add*\n");
                        exit(4);
                    }	
                }
                fprintf(outfile,"0x%0.4x\n", ADD + arg1 + arg2 + arg3);
            }
            else if(strcmp(lOpcode,"brn" )==0){
				retVal = convertBR(symbolTableLen, memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRN+retVal);
            }
            else if(strcmp(lOpcode,"brp" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRP+retVal);
            }
            else if(strcmp(lOpcode,"br" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BR+retVal);
            }
            else if(strcmp(lOpcode,"brz" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRZ+retVal);
            }
            else if(strcmp(lOpcode,"brnz" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRNZ+retVal);
            }
            else if(strcmp(lOpcode,"brzp" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRZP+retVal);
            }
            else if(strcmp(lOpcode,"brnzp" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRNZP+retVal);
            }
			else if(strcmp(lOpcode,"brnp" )==0){
				retVal = convertBR(symbolTableLen,memAddr, lArg1 , lArg2, lArg3);
                fprintf(outfile,"0x%0.4X\n",BRNP+retVal);
            }
            else if(strcmp(lOpcode,"jmp" )==0){
				if(strcmp(lArg2,"\0")!=0 || strcmp(lArg1,"\0") == 0)
				{
					printf("ERROR 4: Unwanted Field or missing field");
					exit(4);
				}else
				{
					retVal = regToNum(lArg1,2);
					fprintf(outfile,"0x%0.4X\n",JMP+retVal);
				}
            }
            else if(strcmp(lOpcode,"jsr" )==0){
				labelFlag = 0;
				if(strcmp(lArg2,"\0")!=0 || strcmp(lArg1,"\0") == 0)
				{
					printf("ERROR 4: Unwanted Field or missing field");
					exit(4);
				}
				if(lArg1[0] == 'x' || lArg1[0] == '#'){
					printf("ERROR 4: not a valid label");
					exit(4);
				}
				for(i =0;i<symbolTableLen;i++)
				{
					if(strcmp(symbolTable[i].label,lArg1)==0)
					{
						retVal = (symbolTable[i].address - memAddr)/2;
						if(retVal >= -1024 && retVal <= 1023)
						{
							if(retVal <0)
							{
								retVal = retVal - 0xFFFFF800;
							}
							fprintf(outfile,"0x%0.4X\n",JSR+retVal);
							labelFlag =1;
							break;
						}
						else
						{
							printf("ERROR 3: OFFSET OUT OF RANGE");
							exit(3);
						}
						
					}
				}
				if(labelFlag == 0)
				{
					printf("ERROR 1: Undefined label!!!");
					exit(1);
				}
            }
            else if(strcmp(lOpcode,"jsrr" )==0){
				if(strcmp(lArg2,"\0")!=0 || strcmp(lArg1,"\0") == 0)
				{
					printf("ERROR 4: Unwanted Field or missing field");
					exit(4);
				}else
				{
					retVal = regToNum(lArg1,2);
					fprintf(outfile,"0x%0.4X\n",JSRR + retVal);
				}
            }
            else if(strcmp(lOpcode,"ldb" )==0){
				if(strcmp(lArg1,"\0")==0 || strcmp(lArg2,"\0")==0 || strcmp(lArg3,"\0")==0 )
				{
					printf("Error 4: Missing Operand\n");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= -32 && retVal <=31)
				{
					if(retVal < 0)
					{
						retVal = retVal - 0xFFFFFFC0;
						
					}
					retVal = retVal+  regToNum(lArg1,1) + regToNum(lArg2,2);/*Need to convert here**/
					fprintf(outfile,"0x%0.4X\n",LDB+retVal);
				}else{
					printf("ERROR 3: Invalid Constant\n");
					exit(3);
				}
            }
            else if(strcmp(lOpcode,"ldw" )==0){
				if(strcmp(lArg1,"\0")==0 || strcmp(lArg2,"\0")==0 || strcmp(lArg3,"\0")==0 )
				{
					printf("Error 4: Missing Operand\n");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= -32 && retVal <=31)
				{
					if(retVal < 0)
					{
						retVal = retVal - 0xFFFFFFC0;
					}
					retVal = retVal+  regToNum(lArg1,1) + regToNum(lArg2,2);
					fprintf(outfile,"0x%0.4X\n",LDW+retVal);
				}else{
					printf("ERROR 3: Invalid Constant");
					exit(3);
				}
            }
            else if(strcmp(lOpcode,"lea" )==0){
				labelFlag =0;
				if(strcmp(lArg1,"\0")==0 || strcmp(lArg2,"\0")==0 || strcmp(lArg3,"\0") !=0)
				{
					printf("ERROR 4: Missing opperand or unwanted field");
					exit(4);
				}
				if(lArg2[0] == 'x' || lArg2[0] == '#'){
					printf("ERROR 4: not a valid label");
					exit(4);
				}
				for(i = 0; i < symbolTableLen;i++)
				{
					if(strcmp(symbolTable[i].label, lArg2)==0)
					{
						retVal = (symbolTable[i].address - memAddr)/2;/****need to convert to 2's complement***/
						if(retVal >= -256 && retVal <= 255)
						{
							if(retVal < 0)
							{
								retVal = retVal - 0xFFFFFE00;
							}
							printf("%x!!!!!!!\n", memAddr);
							retVal = retVal + regToNum(lArg1,1);
							fprintf(outfile,"0x%0.4X\n",LEA + retVal);
							labelFlag = 1;
							break;
						}
						
					}
				}
				if(labelFlag == 0)
				{
					printf("ERROR 1: UNDEFINED LABEL!!!");
					exit(1);
				}
            }
            else if(strcmp(lOpcode,"not" )==0){
				if(strcmp(lArg1,"\0") == 0 || strcmp(lArg2,"\0") == 0 || strcmp(lArg3,"\0") !=0)
				{
					printf("ERROR 4: Missing Operand or Unwanted field");
					exit(4);
				}
				retVal = regToNum(lArg1,1) + regToNum(lArg2,2);
                fprintf(outfile,"0x%0.4X\n",NOT+retVal);
            }
            else if(strcmp(lOpcode,"ret" )==0){
				if(strcmp(lArg1,"\0")!=0)
				{
					printf("ERROR 4: Unwanted Field");
					exit(4);
				}
                fprintf(outfile,"0x%0.4X\n",RET);
            }
            else if(strcmp(lOpcode,"rti" )==0){
				if(strcmp(lArg1,"\0")!=0)
				{
					printf("ERROR 4: Unwanted Field");
					exit(4);
				}
				
                fprintf(outfile,"0x%0.4X\n",RTI);
            }
            else if(strcmp(lOpcode,"lshf" )==0){
				if(strcmp(lArg1,"\0") == 0 || strcmp(lArg2,"\0") == 0||strcmp(lArg3,"\0") == 0)
				{
					printf("ERROR 4: Missing Operand");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= 0 && retVal <= 15)
				{
					retVal = retVal + regToNum(lArg1,1) + regToNum(lArg2,2);/***NEED TO CONVERT TO 2'S complement before adding***/
					fprintf(outfile,"0x%0.4X\n",LSHF+retVal);
				}else
				{
					printf("ERROR 3: INVALID CONSTANT");
					exit(3);
				}
                
            }
            else if(strcmp(lOpcode,"rshfl" )==0){
				if(strcmp(lArg1,"\0") == 0 || strcmp(lArg2,"\0") == 0||strcmp(lArg3,"\0") == 0)
				{
					printf("ERROR 4: Missing Operand");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= 0 && retVal <= 15)
				{
					retVal = retVal + regToNum(lArg1,1) + regToNum(lArg2,2);
					fprintf(outfile,"0x%0.4X\n",RSHFL +retVal);
				}else
				{
					printf("ERROR 3: INVALID CONSTANT");
					exit(3);
				}
            }
            else if(strcmp(lOpcode,"rshfa" )==0){
				if(strcmp(lArg1,"\0") == 0 || strcmp(lArg2,"\0") == 0||strcmp(lArg3,"\0") == 0)
				{
					printf("ERROR 4: Missing Operand");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= 0 && retVal <= 15)
				{
					retVal = retVal + regToNum(lArg1,1) + regToNum(lArg2,2);/***NEED TO CONVERT TO 2'S complement before adding***/
					fprintf(outfile,"0x%0.4X\n",RSHFA+retVal);
				}else
				{
					printf("ERROR 3: INVALID CONSTANT");
					exit(3);
				}
            }
            else if(strcmp(lOpcode,"stb" )==0){
				if(strcmp(lArg1,"\0")==0 || strcmp(lArg2,"\0")==0 || strcmp(lArg3,"\0")==0 )
				{
					printf("Error 4: Missing Operand");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= -32 && retVal <=31)
				{
					
					if(retVal<0)
						retVal = retVal - 0xFFFFFFC0;
					retVal = retVal+  regToNum(lArg1,1) + regToNum(lArg2,2);
					fprintf(outfile,"0x%0.4X\n",STB+retVal);
				}else{
					printf("ERROR 3: Invalid Constant");
					exit(3);
				}
			
            }
            else if(strcmp(lOpcode,"stw" )==0){
				if(strcmp(lArg1,"\0")==0 || strcmp(lArg2,"\0")==0 || strcmp(lArg3,"\0")==0 )
				{
					printf("Error 4: Missing Operand");
					exit(4);
				}
				retVal = toNum(lArg3);
				if(retVal >= -32 && retVal <=31)
				{
					if(retVal < 0)
						retVal = retVal - 0xFFFFFFC0;
					retVal = retVal+  regToNum(lArg1,1) + regToNum(lArg2,2);
					fprintf(outfile,"0x%0.4X\n",STW+retVal);
				}else{
					printf("ERROR 3: Invalid Constant");
					exit(3);
				}
			
            }
            else if(strcmp(lOpcode,"trap" )==0){
				if(strcmp(lArg2,"\0" ) !=0)
				{
					printf("ERROR 4: Unwanted Field \n");
					exit(4);
				}
				if(lArg1[0] == 'x')
				{
					retVal = toNum(lArg1);
					if(retVal >= 0 && retVal <=255)
					{
						fprintf(outfile,"0x%0.4X\n",TRAP+retVal);
					}else{
						printf("ERROR 3: INVALID CONSTANT -OUT OF BOUND \n");
						exit(3);
					}
				}else{
					printf("ERROR 3: INVALID CONSTANT \n");
					exit(3);
				}
            }
            else if(strcmp(lOpcode,"xor" )==0){
                              
                if(strcmp(lArg1,"\0")==0){
                    printf("Error 4!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    arg1 = regToNum(lArg1, 1);
                }
                if(strcmp(lArg2,"\0")==0){
                    printf("Error 4!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    arg2 = regToNum(lArg2, 2);
                }
                
                if(strcmp(lArg3,"\0")==0){
                    printf("Error 4!!! Unwanted Field");
                    exit(4);
                }
                else
                {
                    if(lArg3[0] == 'r')
                    {
                        arg3 = regToNum(lArg3, 3);
                    }
                    else if(lArg3[0] == '#' || lArg3[0] == 'x')
                    {
                        if(toNum(lArg3)>=-16 && toNum(lArg3)<=15)
                        {
							retVal = toNum(lArg3);
							if(retVal <0)
							{
								retVal = retVal - 0xFFFFFFE0;
							}
                            arg3 = toNum(lArg3) + 32;
                        }
                        else
                        {
                            printf("**ERROR 3 Invalid Constant**\n");
                            exit(3);
                        }
                    }
                    else
                    {
                        printf("**ERROR 4 INVALID ARG**\n");
                        exit(4);
                    }	
                }
                fprintf(outfile,"0x%0.4x\n", XOR + arg1 + arg2 + arg3);
            }
            else if(strcmp(lOpcode,"halt" )==0){
                if(strcmp(lArg1,"\0") !=0 || strcmp(lArg2,"\0") !=0 ||  strcmp(lArg3,"\0") !=0 || strcmp(lArg4,"\0") !=0)
                {
                    printf("Halt error");
                    exit(4);
                }
                fprintf(outfile,"0x%0.4X\n",HALT);
            }
         }
    }while(lRet !=DONE);
	if(strcmp(lOpcode,".end")!=0){
		printf("ERROR 4: NO .END \n");
		exit(4);
	}
	return (DONE);
}
/**********END assembleCommand**************/








/**************MAIN*******************/
int main(int argc, char* argv[]){
    char *prgName = NULL;
    char *iFileName = NULL;
    char *oFileName = NULL;
    int lRet,i,j;
    int labelTableLen;
    int memaddr = 0;
	int firstOpcode =0;

    /*Line parsing init*/
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	        *lArg2, *lArg3, *lArg4;
    labelTableLen = 0;
    i=0;
    /*Use for debugging*/
		prgName   = argv[0];
        iFileName = argv[1];
        oFileName = argv[2];
        printf("program name = '%s'\n", prgName);
     	printf("input file name = '%s'\n", iFileName);
     	printf("output file name = '%s'\n", oFileName);
     /*open in file*/
	infile = fopen(argv[1],"r");
	if(!infile){
		printf("Cannot open %s file \n",argv[1]);
		exit(4);	
	}

    do
    {
        lRet = readAndParse( infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
			if( lRet != DONE && lRet != EMPTY_LINE )
			{
				memaddr++;
				memaddr++;
				j=0;
				if(firstOpcode == 0 && strcmp(lOpcode,".orig") !=0)
				{
					printf("ERROR 4: NOT A VALID PROGRAM");
					exit(4);
				}
				/*if(strcmp(lOpcode,"\0") == 0){
					printf("ERROR 4: NO OPCODE");
					exit(4);
				}*/
				/********For Debug*********/
				if(DEBUG_EN){
					i++;
					printf("*****Line: %d ***** \n", i);
					printf("Label: %s \n",lLabel);
					printf("Opcode: %s \n",lOpcode); 
					printf("Arg1: %s \n",lArg1);
					printf("Arg2: %s \n",lArg2);
					printf("Arg3: %s \n",lArg3);
					printf("Arg4: %s \n",lArg4);
					printf("Addr: 0x%x\n\n", memaddr);
				}
				/********EndDebugBlock*****/
			if(memaddr ==1 && strcmp(lOpcode,"\0") == 0)
			{
				printf("ERROR: No starting address");
				exit(4);
			}
            if(isOpcode(lOpcode) == -1 && isPseudoOpcode(lOpcode) == -1)
			{
				printf("**ERROR 2 INVALID OPCODE**\n");
                exit(2); /*Opcode not define in ISA*/
			}
            else if(strcmp(lOpcode, ".orig") == 0)
			{
				firstOpcode = 1;
                printf ("set starting address \n");
				/*if((lArg1[0]=='x') &&(lArg1[4]=='\0' || lArg1[5] !='\0'))
				{
					printf("ERROR 3: INVALID STARTING ADDRESS \n");
					exit(3);
				}*/
				if(toNum(lArg1) > 0xFFFF || toNum(lArg1) <0){
					printf("ERROR 3: INVALID STARTING ADDRESS \n");
					exit(3);
				}
				if(toNum(lArg1)%2 ==0 )
				{
					memaddr = toNum(lArg1)-2;
				}
				else
				{
					printf("**ERROR 3 INVALID CONSTANT**\n");
					exit(3);
				}
			
			}
			
			
			else if(strcmp(lOpcode,".end") == 0){
				if(strcmp(lArg1,"\0")!=0)
				{
					printf("ERROR 4: Unwanted Field");
					exit(4);
				}
				lRet = DONE;
			}
			
			
            /*Add label to constructor*/
			else if(strcmp(lLabel, "\0")!=0 && strcmp(lLabel, "\t\n ,")!=0)
			{
				if(labelTableLen == 255)
				{
					printf("Lable Table is full\n");
					exit(4);
				}
				else
				{
					if(strcmp(lLabel ,"r0") == 0 || strcmp(lLabel ,"r1") == 0 || strcmp(lLabel ,"r2") == 0 ||
					strcmp(lLabel ,"r3") == 0 ||strcmp(lLabel ,"r4") == 0 ||strcmp(lLabel ,"r5") == 0 ||
					strcmp(lLabel ,"r6") == 0 ||strcmp(lLabel ,"r7") == 0||strcmp(lLabel ,"in") == 0
					||strcmp(lLabel ,"out") == 0||strcmp(lLabel ,"getc") == 0 || strcmp(lLabel ,"puts") == 0)
					{
						printf("ERROR 4: Not a Valid label \n");
						exit(4);
					}
					for(j=0; j<labelTableLen; j++)
					{
						if(strcmp(symbolTable[j].label, lLabel) == 0)
						{
							printf("Error 4 Label already exists\n");
							exit(4);
						}
					}
					strcpy(symbolTable[labelTableLen].label, lLabel); /*Need to add address*/
					symbolTable[labelTableLen].address = memaddr;
					labelTableLen++;
				}
			}  
         }
		 
    }while(lRet !=DONE);
    

   /********For Debug*********/
   if(DEBUG_EN){
       printf("Debug: \n");
       for(i = 0;i < labelTableLen;i++){
            printf("Label %d : %s 		Addr: 0x%x \n", i,symbolTable[i].label, symbolTable[i].address);
       }
   }
   
   /*****End Debug****/
	fclose(infile); /*End First pass*/
    printf("Done First pass \n");
    
    infile = fopen(argv[1],"r");
	outfile = fopen(argv[2],"w");
    
	if(assembleCommand(labelTableLen) == DONE)
        printf("******FINISH!!! NO ERROR******* \n");
	fclose(outfile);
	fclose(infile);
    exit(0);
}



