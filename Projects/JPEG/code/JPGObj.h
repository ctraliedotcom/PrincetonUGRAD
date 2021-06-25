/*Programmer: Chris Tralie (ctralie@princeton.edu)*/
/*Purpose: To provide methods that parse information in a JPEG's header,
*and to make a C-Style "class" that encapsulates these methods to actually
*read in and draw a JPEG image to the screen, with the help of some
*other files I've written*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include <math.h>

#ifndef JPGOBJ_H
#define JPGOBJ_H

#define PI 3.141592654

/*Below are the definitions of different markers in the JPEG header*/
#define JPG_SOI					0xFFD8
#define JFIF_MARKER 			0xFFE0
#define DQT_MARKER 				0xFFDB
#define DHT_MARKER 				0xFFC4
#define SOF_MARKER 				0xFFC0
#define SOS_MARKER 				0xFFDA
#define COMMENT_MARKER 			0XFFFE
#define EOI						0xFFD9

/*Below are some more constants defined for processing*/
#define COMPONENT_Y 			1
#define COMPONENT_CB			2
#define COMPONENT_CR			3
/*Information below obtained from 
*http://www.impulseadventure.com/photo/jpeg-color-space.html*/
#define CRED					0.299
#define CGREEN					0.587
#define CBLUE					0.114

/*Opaque pointer*/
typedef struct JPGObj* JPGObj_T;

void JPGObj_free(JPGObj_T jpgObj);

JPGObj_T JPGObj_new(char* filename, int doDCT);

void JPGObj_draw(JPGObj_T jpgObj, HDC* hdc, PAINTSTRUCT* ps);

typedef struct COEFF{
WORD values[64];
} Coefficients;



#endif
