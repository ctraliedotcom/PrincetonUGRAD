/*Programmer: Chris Tralie (ctralie@princeton.edu)*/
/*Purpose: To create a class that can convert information from
*a JPEG file's Huffman Table into an actual binary Huffman Tree, 
*which can then be used to decode compressed binary data.  Note
*that the run-length coding scheme is also implemented in the decode
*data stream section*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "JPGOBj.h"

#ifndef HTREE_H
#define HTREE_H

#define EOB 0x00
typedef struct HTree* HTree_T;

HTree_T HTree_new(BYTE* bits, BYTE* hValues);

/*Free the Huffman Tree from the memory heap*/
void HTree_free(HTree_T oHTree);

void HTree_decodeTest(HTree_T oHTree, BYTE* data, int length, BYTE* decoded);

void HTree_decode(HTree_T dcTable, HTree_T acTable, BYTE* qTable, 
				  Coefficients* coefficients, int index, FILE* file,
				  BYTE* b, BYTE* bitPos);
#endif
