/*Programmer: Chris Tralie (ctralie@princeton.edu)*/
/********************************************************************
 *Define some lower level bit operations that wil come in handy		*
 *******************************************************************/

#include "bitOps.h"



WORD getSmallest(WORD w1, WORD w2){
	if(w1 < w2)
	return w1;
	return w2;	
}
 

/*******************************************************************
 *Define some low level bit operation functions that will be vital *
 *for encoding and decoding                                        *
 *******************************************************************/

/*JPEG variables are stored in big endian format, but this program
 *is written for Intel processors, which are little endian.  Create
 *two functions to swap endianness; one for a WORD (two bytes) and
 *one for a DWORD (four bytes)*/

/*Return a WORD equivalent to value with its endianness swapped*/
WORD endianSwapW(WORD value){
     return (((value & 0xFF00) >> 8) |
            ( (value & 0x00FF) << 8) );
}

/*Return a DWORD equivalent to value with its endianness swapped*/
DWORD endianSwapD(DWORD value){
     return (((value & 0xFF000000) >> 24) |
             ((value & 0x00FF0000) >> 8) |
             ((value & 0x0000FF00) << 8) |
             ((value & 0x000000FF) << 24) );
}

/*Get a WORD from the filestream file, and convert it to little
 *endian format*/
WORD getWord(FILE* file){
	WORD toReturn;
	fread(&toReturn, 2, 1, file);
	toReturn = endianSwapW(toReturn);
	return toReturn;
}

BYTE getByte(FILE* file){
	BYTE toReturn;
	fread(&toReturn, 1, 1, file);
	return toReturn;	
}

/*If the BYTE is 0xFF, check to make sure the next byte is 0x00
 *before returning 0xFF.  Otherwise, totally skip the two 0xFFxx 
 *(probably for alignment purposes, and to be ignored)*/
BYTE getByteF(FILE* file){
	BYTE b = getByte(file);
	if(b == 0xFF){
		b = getByte(file);
		if(b != 0x00)
			return getByte(file);	
		return 0xFF;
	}
	return b;	
}

/*Create functions to extract BYTES from a WORD, as information will
 *often be read from a file a WORD at a time (a marker is a WORD)*/

BYTE getLowBYTE(WORD value){
	BYTE* toReturn = (BYTE*)(&value);
	return *toReturn;
}

BYTE getHighBYTE(WORD value){
	BYTE* toReturn = (BYTE*)(&value);
	toReturn ++;
	return *toReturn;
}

/*A nibble is a hexadecimal digit (four bits).  The low
 *nibble is the least significant nibble of a byte, and the
 *high nibble is the most significant nibble of a byte*/
BYTE getLowNibble(BYTE b){
	return b & 0x0F;	
}

BYTE getHighNibble(BYTE b){
	return (b >> 4);	
}

/*Return the bit from BYTE b at position pos*/
/*NOTE: Bit at position 0 is most significant bit, at position 7
 *is least significant bit*/
BYTE getBit(BYTE b, int pos){
    BYTE arithmetic = 1 << (7-pos);
    b = b & arithmetic;
	return 	(b == 0)? 0:1;
}

void printBinary(BYTE b){
	int i;
	for(i=0; i<8; i++){
		printf("%i ", getBit(b, i));	
	}
	printf("\n");
}

/*Set the bit at position pos of BYTE b to value
 *(value is 1 or 0)*/
void setBit(BYTE* b, int pos, int value){
	BYTE operand =  *b;
	BYTE arithmetic = 1 << (7-pos);
	if(value == 0)
       operand = operand & (arithmetic ^ 0xFF);
    else
       operand = operand | arithmetic;
    *b = operand;
}

BYTE getBitW(WORD w, int pos){
	WORD arithmetic = 1 << (15 - pos);
	w = w & arithmetic;
	return (w == 0)? 0:1;	
}

void setBitW(WORD* w, int pos, int value){
	WORD operand =  *w;
	WORD arithmetic = 1 << (15-pos);
	if(value == 0)
       operand = operand & (arithmetic ^ 0xFFFF);
    else
       operand = operand | arithmetic;
    *w = operand;		
}

void setBitI(int* i, int pos, int value){
	int operand =  *i;
	int arithmetic = 1 << (33-pos);
	if(value == 0)
       operand = operand & (arithmetic ^ 0xFFFFFFFF);
    else
       operand = operand | arithmetic;
    *i = operand;		
}
