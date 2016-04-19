/***************************************************************************
*                     Sample Program Using LZSS Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of LZSS library
*   Author  : Michael Dipperstein
*   Date    : February 21, 2004
*
****************************************************************************
*
* SAMPLE: Sample usage of LZSS Library
* Copyright (C) 2004, 2006, 2007, 2014 by
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
#include <stdlib.h>
#include <string.h>
#include "lzss.h"

#include "lzlocal.h"
/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef enum
{
    ENCODE,
    DECODE
} modes_t;

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : main
*   Description: This is the main function for this program, it validates
*                the command line input and, if valid, it will either
*                encode a file using the LZSS algorithm or decode a
*                file encoded with the LZSS algorithm.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Encodes/Decodes input file
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int main(int argc, char *argv[])
{
    FILE *org;             /* pointer to open input file */
    FILE *comp;            /* pointer to open output file */
	FILE *decomp;            /* pointer to open output file */
	printf("WINDOW_SIZE: %d  MAX_CODED: %d \n",WINDOW_SIZE,MAX_CODED);

    /* initialize data */
    org = NULL;
    comp = NULL;
	decomp = NULL;
	/* open input file as binary */
                org = fopen("org.txt", "rb"); 
                if (org == NULL)
                {
                    perror("Opening input file");
				}

				/* open output file as binary */
                 comp = fopen("comp.txt", "wb"); 
				/*fpOut = fopen("in.txt", "wb");*/
                if (comp == NULL)
                {
                    perror("Opening output file");
				}
printf("Encoding.....\n");
	 EncodeLZSS(org, comp); 
/*   DecodeLZSS(fpIn, fpOut);*/
/* remember to close files */
    fclose(org);
    fclose(comp);




	/* open input file as binary */
                comp = fopen("comp.txt", "rb"); 
				/*fpIn = fopen("out.txt", "rb");*/
                if (comp == NULL)
                {
                    perror("Opening input file");
				}

				/* open output file as binary */
                 decomp = fopen("decomp.txt", "wb"); 
				/*fpOut = fopen("in.txt", "wb");*/
                if (decomp == NULL)
                {
                    perror("Opening output file");
				}
printf("\n");
printf("Decoding.....\n");
 Decode(comp, decomp); 
 printf("\n");
 printf("\n");
 /*   DecodeLZSS(fpIn, fpOut);*/
/* remember to close files */
    fclose(comp);
    fclose(decomp);


	/* open input file as binary */
                org = fopen("org.txt", "rb"); 
				/*fpIn = fopen("out.txt", "rb");*/
                if (org == NULL)
                {
                    perror("Opening input file");
				}

				/* open output file as binary */
                 decomp = fopen("decomp.txt", "rb"); 
				/*fpOut = fopen("in.txt", "wb");*/
                if (decomp == NULL)
                {
                    perror("Opening output file");
				}
printf("Checking.....\n");
 diff(org, decomp); 
 
/* remember to close files */
    fclose(comp);
    fclose(decomp);

	printf("\n");
    return 0;
}


