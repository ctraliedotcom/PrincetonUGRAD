/*Programmer: Chris Tralie (ctralie@princeton.edu)*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef BITOPS_H
#define BITOPS_H

WORD getSmallest(WORD w1, WORD w2);

WORD endianSwapW(WORD value);

DWORD endianSwapD(DWORD value);

WORD getWord(FILE* file);

BYTE getByte(FILE* file);

BYTE getByteF(FILE* file);

BYTE getLowBYTE(WORD value);

BYTE getHighBYTE(WORD value);

BYTE getLowNibble(BYTE b);

BYTE getHighNibble(BYTE b);

BYTE getBit(BYTE b, int pos);

void printBinary(BYTE b);

void setBit(BYTE* b, int pos, int value);

BYTE getBitW(WORD w, int pos);

void setBitW(WORD* w, int pos, int value);

void setBitI(int* i, int pos, int value);

#endif
