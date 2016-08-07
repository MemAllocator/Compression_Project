#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lzlocal.h"
#include "bitfile.h"
#include <stdlib.h>
#include "CompressedMatching.h"

int CompressionMatching(FILE *fpIn, char *pat)
{
	bit_file_t *bfpIn;
	int c;
	unsigned int i, index;
	encoded_string_t code;
	char *buff;
	unsigned int current = 1;
	unsigned int k = 1;
	unsigned int relevant = 0;


	/*unsigned char text[ARRAY_SIZE];*/
	unsigned char buf[WINDOW_SIZE];
	/* use stdin if no input file */
	if ((NULL == fpIn))
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
	for (i = 1; i < WINDOW_SIZE; i++) {
		buff[i-1] = 0;
	}
	index = 0;
	while (c = (BitFileGetBit(bfpIn)) != EOF || buff[current]!=0)
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
			while (buff[index] != 0) {
				index++;
			}


			/* write out byte and put it in sliding window */
			buff[index] = c;
			index++;

		}
		else
		{
			while (c == ENCODED) {
				/* offset and length */
				code.offset = 0;
				code.length = 0;
				code.slide = 0;
				if ((BitFileGetBitsNum(bfpIn, &code.offset, OFFSET_BITS, sizeof(unsigned int))) == EOF)
				{
					break;
				}

				if ((BitFileGetBitsNum(bfpIn, &code.length, LENGTH_BITS, sizeof(unsigned int))) == EOF)
				{
					break;
				}
				if ((BitFileGetBitsNum(bfpIn, &code.slide, SLIDE_BITS, sizeof(unsigned int))) == EOF)
				{
					break;
				}
				/*	printf("(%d, %d ,%d) ",code.offset,code.length,code.slide);      */

				for (i = 0; i < code.length; i++)
				{
					buff[index + code.offset + code.slide + i] = buff[index - code.length + code.slide + i];
				}

			}
	}

	}
	BitFileToFILE(bfpIn);
	return 0;

}

void KMPSearch(char *pat, char *txt)
{
	int M = strlen(pat);
	int N = strlen(txt);

	// create lps[] that will hold the longest prefix suffix values for pattern
	int *lps = (int *)malloc(sizeof(int)*M);
	int j = 0;  // index for pat[]

				// Preprocess the pattern (calculate lps[] array)
	computeLPSArray(pat, M, lps);

	int i = 0;  // index for txt[]
	while (i < N)
	{
		if (pat[j] == txt[i])
		{
			j++;
			i++;
		}

		if (j == M)
		{
			printf("Found pattern at index %d \n", i - j);
			j = lps[j - 1];
		}

		// mismatch after j matches
		else if (pat[j] != txt[i])
		{
			// Do not match lps[0..lps[j-1]] characters,
			// they will match anyway
			if (j != 0)
				j = lps[j - 1];
			else
				i = i + 1;
		}
	}
	free(lps); // to avoid memory leak
}

void computeLPSArray(char *pat, int M, int *lps)
{
	int len = 0;  // lenght of the previous longest prefix suffix
	int i;

	lps[0] = 0; // lps[0] is always 0
	i = 1;

	// the loop calculates lps[i] for i = 1 to M-1
	while (i < M)
	{
		if (pat[i] == pat[len])
		{
			len++;
			lps[i] = len;
			i++;
		}
		else // (pat[i] != pat[len])
		{
			if (len != 0)
			{
				// This is tricky. Consider the example AAACAAAA and i = 7.
				len = lps[len - 1];

				// Also, note that we do not increment i here
			}
			else // if (len == 0)
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}

// Driver program to test above function
int main()
{
	char *txt = "apurba mandal loves ayoshi loves";
	char *pat = "loves";
	KMPSearch(pat, txt);
	return 0;
}