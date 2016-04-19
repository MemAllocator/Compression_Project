/***************************************************************************
*                 Lempel, Ziv, Storer, and Szymanski Encoding
*
*   File    : lzss.c
*   Purpose : Use lzss coding (Storer and Szymanski's modified LZ77) to
*             compress lzss data files.
*   Author  : Michael Dipperstein
*   Date    : November 28, 2014
*
****************************************************************************
*
* LZss: An ANSI C LZSS Encoding/Decoding Routines
* Copyright (C) 2003 - 2007, 2014 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lzlocal.h"
#include "bitfile.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */

unsigned char slidingWindow[WINDOW_SIZE];
unsigned char Look[MAX_CODED];
encoded_string_t Matchs[WINDOW_SIZE/(MAX_UNCODED+1)];
/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : EncodeLZSS
*   Description: This function will read an input file and write an output
*                file encoded according to the traditional LZSS algorithm.
*                This algorithm encodes strings as 16 bits (a 12 bit offset
*                + a 4 bit length).
*   Parameters : fpIn - pointer to the open binary file to encode
*                fpOut - pointer to the open binary file to write encoded
*                       output
*   Effects    : fpIn is encoded and written to fpOut.  Neither file is
*                closed after exit.
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int EncodeLZSS(FILE *fpIn, FILE *fpOut)
{
	bit_file_t *bfpOut;
	//encoded_string_t matchData;
	int c;
	unsigned int i ,j;
	unsigned int len;                       
	unsigned int count;
	unsigned int temp;
	/* head of sliding window and lookahead */
	unsigned int windowHead, LookHead;
	/* array of flags that told us for each characher if it replaced with a pointer */
	unsigned int flagLook[ MAX_CODED ];
	unsigned int flagWindow[ WINDOW_SIZE ];

	
	/* validate arguments */
	if ((NULL == fpIn) || (NULL == fpOut))
	{
		errno = ENOENT;
		return -1;
	}

	/* convert output file to bitfile */
	bfpOut = MakeBitFile(fpOut, BF_WRITE);

	if (NULL == bfpOut)
	{
		perror("Making Output File a BitFile");
		return -1;
	}

	/* Initialized flag array*/
	for( i = 0 ; i <  WINDOW_SIZE ; i++ ){  
		flagWindow[i] = UNCODED;
	}
	for( i = 0 ; i <  MAX_CODED ; i++ ){  
		flagLook[i] = UNCODED;
	}

	windowHead = 0;
	LookHead = 0;

  /************************************************************************
    * Fill the sliding window buffer with some known vales.  DecodeLZSS must
    * use the same values.  If common characters are used, there's an
    * increased chance of matching to the earlier strings.
    ************************************************************************/
/*	memset(Look, ' ', MAX_CODED * sizeof(unsigned char));*/



	
	for (len = 0; len < MAX_CODED && (c = getc(fpIn)) != EOF; len++)
	{
		Look[len] = c;
		BitFilePutBit(UNCODED, bfpOut);
		BitFilePutChar(c, bfpOut);
		/*printf(" %c ",c);*/
	}

	if (0 == len)
	{
		return 0;    
	}
	
	for (len = 0; len < WINDOW_SIZE && (c = getc(fpIn)) != EOF; len++)
	{
		slidingWindow[len] = c;

	}

	if (0 == len)
	{
		return 0;   /* inFile was too short */
	}


	/* now encoded the rest of the file until an EOF is read */
	while (1)
	{
		len = FindMatch(windowHead, LookHead);

		for( i = 0 ; i < len ; i++){ /* run over all the matches that has been found */
			count=0;
			/* check the Match for flaging characters */
			for( j = 0 ; j < Matchs[i].length ; j++){
				if(flagWindow[Wrap((  Matchs[i].offset + j ),WINDOW_SIZE )] == ENCODED ){
					count++;
				}

			}
			if(count == 0){
			


/*
			if( (Matchs[i].length < MAX_CODED - 1) && (slidingWindow[Wrap((windowHead),WINDOW_SIZE)]  == slidingWindow[Wrap((Matchs[i].offset + Matchs[i].length),WINDOW_SIZE)]) && (Wrap((Matchs[i].offset + Matchs[i].length),WINDOW_SIZE) != windowHead)){ 
				break;
			}
*/
			if( Matchs[i].length > MAX_UNCODED ){ /*  only if the match is bigger than MAX_UNCODED we will replace it with pointer  */
				
				/*
				for(j = 0; j < count; j++){*//* find slide (in case we succeced to find better match) */
					/*if(flagWindow[Wrap((windowHead + j), WINDOW_SIZE)] == ENCODED ){
						Matchs[i].slide++;
					}
				}
				*/
				/*
				if( Matchs[i].length >= MAX_CODED -1){
					Matchs[i].length = MAX_CODED - 1;
				}
				*/
				for( j = 0 ; j < (Matchs[i].length ); j++){  /* find slide  */ /* j < (Matchs[i].length - count */
					if(flagLook[Wrap(( LookHead + MAX_CODED - 1 - j ),MAX_CODED )] == ENCODED ){
						Matchs[i].slide++;
					}
					else{
						break;
					}
				}
				for( j = 0 ; j < Matchs[i].length ; j++){ /*update flagWindows for the current match */ 
					flagWindow[ Wrap(( Matchs[i].offset + j), WINDOW_SIZE)] = ENCODED;
				}
				temp = Wrap((Matchs[i].offset + WINDOW_SIZE - windowHead - count ),WINDOW_SIZE);
				printf("(%d, %d ,%d) ",temp,Matchs[i].length,Matchs[i].slide); 
				BitFilePutBit(ENCODED, bfpOut);
				BitFilePutBitsNum(bfpOut, &temp, OFFSET_BITS,sizeof(unsigned int));
				BitFilePutBitsNum(bfpOut, &(Matchs[i].length), LENGTH_BITS,sizeof(unsigned int));
				BitFilePutBitsNum(bfpOut, &(Matchs[i].slide), SLIDE_BITS,sizeof(unsigned int));
			}
		}
		}
		if(flagWindow[windowHead] == UNCODED){/* if the next char has no pointer then we need to write him to file*/ 
		/*	printf("%c ",slidingWindow[windowHead]); */
			BitFilePutBit(UNCODED, bfpOut);
			BitFilePutChar(slidingWindow[windowHead], bfpOut);
		}

		/*  updating slidingWindow, Look and flag arrays. */

		Look[LookHead] = slidingWindow[windowHead];
		flagLook[LookHead] = flagWindow[windowHead];
		slidingWindow[windowHead] = c = getc(fpIn);
		flagWindow[windowHead] = UNCODED; /* new char so there is no pointer yet*/

		windowHead = Wrap((windowHead + 1), WINDOW_SIZE);
		LookHead = Wrap((LookHead + 1), MAX_CODED);

		/* handle case where we hit EOF before filling lookahead */
		if( c == EOF ){
			/*write all  the remaining chars in Windows that dont have pointer to bfpOut */
			for( i = 0; i < WINDOW_SIZE-1 ; i++ ){
				if( flagWindow [Wrap(( i + windowHead ) ,  WINDOW_SIZE ) ] == UNCODED ){
					BitFilePutBit(UNCODED, bfpOut);
					BitFilePutChar(slidingWindow[ Wrap ((i + windowHead), WINDOW_SIZE )], bfpOut);
				/*	printf("%c ", slidingWindow[ Wrap ((i + windowHead), WINDOW_SIZE )]); */

				}
			}
			printf("\n");	
			break;
		}        
	}
	/* we've encoded everything, free bitfile structure */
	BitFileToFILE(bfpOut);
	return 0;
}

/****************************************************************************
*   Function   : DecodeLZSSByFile
*   Description: This function will read an LZSS encoded input file and
*                write an output file.  This algorithm encodes strings as 16
*                bits (a 12 bit offset + a 4 bit length).
*   Parameters : fpIn - pointer to the open binary file to decode
*                fpOut - pointer to the open binary file to write decoded
*                       output
*   Effects    : fpIn is decoded and written to fpOut.  Neither file is
*                closed after exit.
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int DecodeLZSS(FILE *fpIn, FILE *fpOut)
{
	bit_file_t *bfpIn;
	int c;
	unsigned int i, index;
	encoded_string_t code;              /* offset/length code for string */
	
	unsigned char buffer[WINDOW_SIZE];
	unsigned char text[
		WINDOW_SIZE];
	/* use stdin if no input file */
	if ((NULL == fpIn) || (NULL == fpOut))
	{
		errno = ENOENT;
		return -1;
	}

	/* convert input file to bitfile */
	bfpIn = MakeBitFile(fpIn, BF_READ);

	if (NULL == bfpIn)
	{
		perror("Making Input File a BitFile");
		return -1;
	}
	for(i = 0 ; i < WINDOW_SIZE ; i++){
		buffer[i] = 0 ;
		text[i] = 0 ;
	}
	index = 0;
	while (1)
	{
		
		
			if ((c = BitFileGetBit(bfpIn)) == EOF)
			{
				/* we hit the EOF */
				break;
			}

			if (c == UNCODED)
			{
				/* uncoded character */
				if ((c = BitFileGetChar(bfpIn)) == EOF)
				{
					break;
				}
				while(1){/* write the characters in the buffer if avliable*/
				if(buffer[index] != 0){/*if ther is character in the buffer*/
					putc(buffer[index], fpOut);
		/*			printf(" %c ", buffer[index]);*/       
					text[index] = buffer[index];
					buffer[index] = 0;
					index = Wrap((index + 1), WINDOW_SIZE);
				}
				else{
					break;
				}
		}


				/* write out byte and put it in sliding window */
				putc(c, fpOut);
				text[index] = c;
				index = Wrap((index + 1), WINDOW_SIZE);
				printf("%c ", c);   
			}
			else
			{
				/* offset and length */
				code.offset = 0;
				code.length = 0;
				code.slide = 0;
				if ((BitFileGetBitsNum(bfpIn, &code.offset, OFFSET_BITS,sizeof(unsigned int))) == EOF)
				{
					break;
				}

				if ((BitFileGetBitsNum(bfpIn, &code.length, LENGTH_BITS,sizeof(unsigned int))) == EOF)
				{
					break;
				}
				if ((BitFileGetBitsNum(bfpIn, &code.slide, SLIDE_BITS,sizeof(unsigned int))) == EOF)
				{
					break;
				}
				/* code.length += MAX_UNCODED + 1; */

				/****************************************************************
				* Write out decoded string to file and lookahead.  It would be
				* nice to write to the sliding window instead of the lookahead,
				* but we could end up overwriting the matching string with the
				* new string if abs(offset - next char) < match length.
				****************************************************************/
				printf("(%d, %d ,%d) ",code.offset,code.length,code.slide);        
				if(code.length > code.slide){
					for (i = 0; i < (code.length - code.slide); i++)
					{

							buffer[Wrap((index + code.offset + code.slide + i),WINDOW_SIZE)] = text[Wrap((index + WINDOW_SIZE - code.length + code.slide + i ),WINDOW_SIZE) ];
					}

					for (; i < code.length; i++)   /* take the other chracter from buffer. this chracter still not moved to text because of the slide */
					{
						buffer[Wrap((index + code.offset + code.slide + i),WINDOW_SIZE)] = buffer[Wrap((index + WINDOW_SIZE - code.length + code.slide + i ),WINDOW_SIZE) ];
					}
			}
				else{ /* all characters in the buffer */
						for (i = 0; i < code.length; i++)   /* take the other chracter from buffer. this chracte */
					{

						buffer[Wrap((index + code.offset + code.slide + i),WINDOW_SIZE)] = buffer[Wrap((index + WINDOW_SIZE - code.length + code.slide + i ),WINDOW_SIZE) ];
					}

				}
			}
	}

	/* we've decoded everything, free bitfile structure */
	BitFileToFILE(bfpIn);
	return 0;

}

void printMatch(){
	unsigned int index=0;
	while(1){
		if(Matchs[index].length >0){
			printf("(  %d , %d )", Matchs[index].offset, Matchs[index].length);
			index++;
		}
		else{
			printf("\n");
			break;
		}
	}
}

void diff(FILE *fpIn1, FILE *fpIn2){
	unsigned int C1 , C2;
	unsigned int flag=0;

	while((C1 = getc(fpIn1)) != EOF){
		C2 = getc(fpIn2);
		if(C1 != C2){
			flag = 1;
			break;
		}
	}
	C2 = getc(fpIn2);
	if(C1 != C2){
		flag = 1;
	}
	if(flag == 0){
		printf("The files are the same\n");
	}
	else{
		printf("The files are not the same !!\n");
	}

}

/*
int main(){

printf("WINDOW_SIZE: %d  MAX_CODED: %d \n",WINDOW_SIZE,MAX_CODED);



slidingWindow[0]='e';
slidingWindow[1]='a';
slidingWindow[2]='b';
slidingWindow[3]='c';
slidingWindow[4]='d';
slidingWindow[5]='d';
slidingWindow[6]='e';
slidingWindow[7]='a'; 

Look[0]='a';
Look[1]='b';
Look[2]='c';
Look[3]='d';

FindMatch(0,0);
printMatch();


}
*/

int Decode(FILE *fpIn, FILE *fpOut)
{
	bit_file_t *bfpIn;
	int c;
	unsigned int i, index;
	encoded_string_t code;              /* offset/length code for string */
	
	unsigned char text[ARRAY_SIZE];
	/* use stdin if no input file */
	if ((NULL == fpIn) || (NULL == fpOut))
	{
		errno = ENOENT;
		return -1;
	}

	/* convert input file to bitfile */
	bfpIn = MakeBitFile(fpIn, BF_READ);

	if (NULL == bfpIn)
	{
		perror("Making Input File a BitFile");
		return -1;
	}
	for(i = 0 ; i < ARRAY_SIZE ; i++){
		text[i] = 0 ;
	}
	index = 0;
	while (1)
	{
		
		
			if ((c = BitFileGetBit(bfpIn)) == EOF)
			{
				/* we hit the EOF */
				break;
			}

			if (c == UNCODED)
			{
				/* uncoded character */
				if ((c = BitFileGetChar(bfpIn)) == EOF)
				{
					break;
				}
				while(text[index] != 0){
					index++;
		}


				/* write out byte and put it in sliding window */
				text[index] = c;
				index ++;
				   
			}
			else
			{
				/* offset and length */
				code.offset = 0;
				code.length = 0;
				code.slide = 0;
				if ((BitFileGetBitsNum(bfpIn, &code.offset, OFFSET_BITS,sizeof(unsigned int))) == EOF)
				{
					break;
				}

				if ((BitFileGetBitsNum(bfpIn, &code.length, LENGTH_BITS,sizeof(unsigned int))) == EOF)
				{
					break;
				}
				if ((BitFileGetBitsNum(bfpIn, &code.slide, SLIDE_BITS,sizeof(unsigned int))) == EOF)
				{
					break;
				}
				printf("(%d, %d ,%d) ",code.offset,code.length,code.slide);        
				
					for (i = 0; i <code.length; i++)
					{
						text[index + code.offset + code.slide + i] = text[index - code.length + code.slide + i ];
					}

			
				}
			
	}

	/* we've decoded everything, free bitfile structure */
	for( i = 0 ; i < index; i++){
		putc(text[i], fpOut);

	}
	BitFileToFILE(bfpIn);
	return 0;

}