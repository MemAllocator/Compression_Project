
#ifndef _COMPRESSED_MATCHING_H
#define _COMPRESSED_MATCHING_H

#include <stdio.h>
#include <string.h>

int CompressionMatching(FILE *fpIn, char *pat);
void computeLPSArray(char *pat, int M, int *lps);
void KMPSearch(char *pat, char *txt);



#endif      /* ndef _COMPRESSED_MATCHING_H */