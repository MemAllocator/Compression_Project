/***************************************************************************
*          Lempel, Ziv, Storer, and Szymanski Encoding and Decoding
*
*   File    : brute.c
*   Purpose : Implement brute force matching of uncoded strings for LZSS
*             algorithm.
*   Author  : Michael Dipperstein
*   Date    : February 18, 2004
*
****************************************************************************
*
* Brute: Brute force matching routines used by LZSS Encoding/Decoding
*        Routine
* Copyright (C) 2004 - 2007, 2014 by
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
#include "lzlocal.h"
#include <stdio.h>
/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */
extern unsigned char slidingWindow[];
extern unsigned char Look[];
extern encoded_string_t Matchs[];
/*
unsigned char slidingWindow[WINDOW_SIZE];
unsigned char Look[MAX_CODED];
encoded_string_t Matchs[WINDOW_SIZE/(MAX_UNCODED+1)];*/ /* the max match could be. */

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : InitializeSearchStructures
*   Description: This function initializes structures used to speed up the
*                process of mathcing uncoded strings to strings in the
*                sliding window.  The brute force search doesn't use any
*                special structures, so this function doesn't do anything.
*   Parameters : None
*   Effects    : None
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int InitializeSearchStructures(void)
{
    return 0;
}

/****************************************************************************
*   Function   : FindMatch
*   Description: This function will search through the slidingWindow
*                dictionary for the longest sequence matching the MAX_CODED
*                long string stored in uncodedLookahed.
*   Parameters : windowHead - head of sliding window
*                LoopHead - head of Look buffer
*   Effects    : None
*   Returned   : The sliding window index where the match starts and the
*                length of the match.  If there is no match a length of
*                zero will be returned.
****************************************************************************/

unsigned int FindMatch(const unsigned int windowHead,unsigned int LookHead){
    unsigned int i;
    unsigned int j;
    unsigned int NumOfMatch=0;

	i = windowHead;  /* start at the beginning of the sliding window */
    j = 0;
    while (1)
    {
    	Matchs[NumOfMatch].length = 0;
    	Matchs[NumOfMatch].offset = 0;
		Matchs[NumOfMatch].slide = 0;
		if (slidingWindow[i] == Look[ Wrap((LookHead + (MAX_CODED-1) ),MAX_CODED)])
        {
            /* we matched one. how many more match? */
            j = 1;

			while(slidingWindow[Wrap((i + (WINDOW_SIZE - j )), WINDOW_SIZE)] == Look[Wrap(((LookHead+MAX_CODED - 1) + (- j )), MAX_CODED)]) {
                if (j >= MAX_CODED -1){/* || Wrap((i + (WINDOW_SIZE - j )), WINDOW_SIZE) == windowHead || Wrap(((LookHead+MAX_CODED - 1) + ( - j )), MAX_CODED) == LookHead ) /*
                {                    /* we wrapped around slidingWindow */        /* we wrapped around Look */ 
                    break;
                }
                j++; 
            }
		    	if(j > MAX_UNCODED){
		    		Matchs[NumOfMatch].length = j;
					Matchs[NumOfMatch].offset = Wrap(i+WINDOW_SIZE-j+1,WINDOW_SIZE);
		    		/*printf("(  %d , %d )", Matchs[NumOfMatch].offset , Matchs[NumOfMatch].length);*/
		    		NumOfMatch++;
				}
		    	
        }
        i = Wrap((i + 1), WINDOW_SIZE);
        if (i == windowHead)
        {
            /* we wrapped around */
            break;
        }
	}
	/*printf("\n");*/
	return NumOfMatch;
}


/*

/****************************************************************************
*   Function   : ReplaceChar
*   Description: This function replaces the character stored in
*                slidingWindow[charIndex] with the one specified by
*                replacement.
*   Parameters : charIndex - sliding window index of the character to be
*                            removed from the linked list.
*   Effects    : slidingWindow[charIndex] is replaced by replacement.
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int ReplaceChar(const unsigned int charIndex, const unsigned char replacement)
{
    slidingWindow[charIndex] = replacement;
    return 0;
}


int mainN (){
	unsigned int index=0;
	printf("WINDOW_SIZE: %d  MAX_CODED: %d \n",WINDOW_SIZE,MAX_CODED);
	/*slidingWindow[0]='a';
	slidingWindow[1]='d';
	slidingWindow[2]='g';
	slidingWindow[3]='f';
	slidingWindow[4]='e';
	slidingWindow[5]='d';
	slidingWindow[6]='c';
	slidingWindow[7]='b';
	
	Look[0]='d';
	Look[1]='c';
	Look[2]='b';
	Look[3]='a';
	
	slidingWindow[0]='g';
	slidingWindow[1]='g';
	slidingWindow[2]='g';
	slidingWindow[3]='g';
	slidingWindow[4]='g';
	slidingWindow[5]='d';
	slidingWindow[6]='g';
	slidingWindow[7]='g';
	
	Look[0]='g';
	Look[1]='d';
	Look[2]='g';
	Look[3]='g';*/

/*	
	slidingWindow[0]='e';
	slidingWindow[1]='a';
	slidingWindow[2]='b';
	slidingWindow[3]='c';
	slidingWindow[4]='a';
	slidingWindow[5]='b';
	slidingWindow[6]='c';
	slidingWindow[7]='e';
	
	Look[0]='a';
	Look[1]='b';
	Look[2]='c';
	Look[3]='d';

	*/
	
	/*	
	slidingWindow[0]='e';
	slidingWindow[1]='c';
	slidingWindow[2]='b';
	slidingWindow[3]='a';
	slidingWindow[4]='c';
	slidingWindow[5]='b';
	slidingWindow[6]='a';
	slidingWindow[7]='e';
	
	Look[0]='d';
	Look[1]='c';
	Look[2]='b';
	Look[3]='a';
*/
	slidingWindow[0]='b';
	slidingWindow[1]='a';
	slidingWindow[2]='e';
	slidingWindow[3]='e';
	slidingWindow[4]='c';
	slidingWindow[5]='b';
	slidingWindow[6]='a';
	slidingWindow[7]='c'; /*FindO(3,1);*/
	
	Look[0]='c';
	Look[1]='b';
	Look[2]='a';
	Look[3]='d';
	
	FindMatch(3,3);
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
	
	
	return 1;
}
