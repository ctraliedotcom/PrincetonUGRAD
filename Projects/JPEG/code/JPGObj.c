/*Programmer: Chris Tralie (ctralie@princeton.edu)*/
/*Purpose: To provide methods that parse information in a JPEG's header,
*and to make a C-Style "class" that encapsulates these methods to actually
*read in and draw a JPEG image to the screen, with the help of some
*other files I've written*/

#include "JPGObj.h"
#include "HTree.h"
#include "bitOps.h"

static BOOL readData(JPGObj_T jpgObj, FILE* file);

/*Color space conversion equations obtained from
 *http://www.impulseadventure.com/photo/jpeg-color-space.html*/
static void toYCbCr(BYTE R, BYTE G, BYTE B, BYTE* Y, BYTE* Cb, BYTE* Cr){
	double r = (double)R;
	double g = (double)G;
	double b = (double)B;
	double y, cb, cr;
	
	y = CRED*r + CGREEN*g + CBLUE*b;
	cb = (b - y)/(2 - 2*CRED);
	cr = (r - y)/(2 - 2*CRED);
	*Y = (BYTE)y;
	*Cb = (BYTE)cb;
	*Cr = (BYTE)cr;
}

static void toRGB(double y, double cb, double cr, BYTE* R, BYTE* G, BYTE* B){
	double r, g, b;
	
	r = y + 1.402*(cr - 128.0);
	g = y - 0.34414*(cb - 128.0) - 0.71414*(cr - 128.0);
	b = y + 1.772*(cb - 128.0);
	if(r>255)         r = 255;
    if(r<0)           r = 0;
    if(g>255)         g = 255;
    if(g<0)           g = 0;
    if(b>255)         b = 255;
    if(b<0)           b = 0;
	/*printf("(%f, %f, %f)\n", r, g, b);*/
	*R = (BYTE)r;
	*G = (BYTE)g;
	*B = (BYTE)b;
}

/*******************************************************************
 *Define all of the different portions of the JPEG header          *
 *Pertinent information obtained from							   *
 *http://www.obrador.com/essentialjpeg/headerinfo.htm              *
 *NOTE: For alignment purposes, I often use the					   *
 *__attribute__((packed)) option.  This is so that I can read in   *
 *a big stream of bytes at once and be assured that there are no   *
 *stuff bytes that the structure is using in between them (it will *
 *save code in the long run)									   *
 *******************************************************************/
 
struct JPGHeader_JFIF{
	WORD length;
	char identifier[5]; /*Should read JFIF\0 */
	WORD version;	
	BYTE units;
	/*As quoted from the web site about the units variable:
	 *0 => no units, X and Y specify pixel aspect ratio
	 *1 => X and Y are dots per inch
	 *2 => X and Y are dots per cm  */
	WORD Xdensity;
	WORD Ydensity;
	BYTE Xthumbnail;
	BYTE Ythumbnail;
} __attribute__((packed));



struct QTable{
	BYTE pindex; /*Precision higher four bits, index lower four bits*/
	/*8-bit for precision 0, 16 bit for precision 1*/
	BYTE qValues[64]; /*Quantization table, stored in zigzag format*/
} __attribute__((packed));

struct JPGHeader_QTABLES{
	BYTE num;
	struct QTable* tables;
}__attribute__((packed));


/*NOTE: A DHT segment can contain multiple headers, each with
 *their own index, bits, and huffman values, or they can all be
 *defined separately*/
struct HuffmanTable{
	/*index: high nibble (0) DC table (1) AC Table
	 *low nibble: ID*/
	BYTE index;
	/*bits: How many different codewords of each length there are,
	 *starting at length 1*/
	BYTE bits[16]; 
	/*The Huffman values associated with each codeword.  The
	 *number of bytes is the sum of all of the values in bits
	 *(we need a byte to represent the value of every codeword
	 *that has a mapping in this table)*/
	BYTE bindings;
	BYTE* hValues;
	HTree_T hTree;
}__attribute__((packed));

struct JPGHeader_HUFFMANTABLES{
	struct HuffmanTable tables[4];
	BYTE read;
};

/*Start of frame header*/
struct JPGHeader_SOF{
	WORD length;
	BYTE P; /*Sample precision in bits*/
	WORD height;
	WORD width;
	BYTE components;
	/*The following variables contain information about
	 *the different components of the image.  There is 1 component
	 *for a grayscale image and 3 components for a color image.
	 *ID variable: The ID of the component
	 *HVSample: Horizontal sampling rate (high nibble) and vertical
	 *sampling rate (low nibble)
	 *QTable: Quantization table number*/
	/*NOTE: YCbCr color used*/
	BYTE c1ID;
	BYTE c1HVSample;
	BYTE c1QTable;
	BYTE c2ID;
	BYTE c2HVSample;
	BYTE c2QTable;
	BYTE c3ID;
	BYTE c3HVSample;
	BYTE c3QTable;
}__attribute__((packed));

struct JPGHeader_SOS{
	WORD length;
	BYTE components;
	BYTE c1ID;
	BYTE c1HTable;/*DC (high nibble) and AC (low nibble) table numbers*/
	BYTE c2ID;
	BYTE c2HTable;
	BYTE c3ID;
	BYTE c3HTable;
	BYTE Ss;
	BYTE Se;
	BYTE AhAl;
};

struct JPGHeader_COMMENT{
	WORD length;
	char* string;	
};

/*This structure will bring together a lot of the information from
 *the header and organize it better*/
struct blockObject{
	BYTE hSample;
	BYTE vSample;
	HTree_T dcTree;
	HTree_T acTree;
	struct QTable* qTable;
};

struct JPGObj{
	struct JPGHeader_JFIF 				JFIFHeader;
	struct JPGHeader_QTABLES 			qTables;
	struct JPGHeader_HUFFMANTABLES		hTables;
	struct JPGHeader_SOF				SOF;
	struct JPGHeader_SOS				SOS;
	struct JPGHeader_COMMENT			comment;
	BYTE*								thumbnail;
	BOOL 								grayscale;
	/*Maximum image size is 800 x 800 (800x800 / 64 = 10000)*/
	Coefficients						coefficients[3][10000];
	int 								mcuX, mcuY;
	int 								greatestVSample, greatestHSample;
	struct blockObject					components[3];
	double                              pixels[3][10000][8][8];
	int                                 doDCT;
};



/*******************************************************************
 *Define functions that read in each part of the header from a	   *
 *file stream													   *
 *******************************************************************/
static void readJFIF(JPGObj_T obj, FILE* file){
	WORD length;
	struct JPGHeader_JFIF* jfif = &(obj->JFIFHeader);
	memset(jfif, 0, sizeof(struct JPGHeader_JFIF));
	length = getWord(file);
	fseek(file, -2, SEEK_CUR);
	/*To prevent buffer overrun attacks*/
	if(length > sizeof(struct JPGHeader_JFIF)){
		fprintf(stderr, "Warning: JFIF Header is too big\n");
		length = sizeof(struct JPGHeader_JFIF);	
	}
	fread( jfif, length, 1, file);
	/*Words version, XDensity, YDensity*/
	jfif->version = endianSwapW(jfif->version);
	jfif->Xdensity = endianSwapW(jfif->Xdensity);
	jfif->Ydensity = endianSwapW(jfif->Ydensity);
}

static BOOL readDQT(JPGObj_T obj, FILE* file){
	WORD length = getWord(file);
	struct JPGHeader_QTABLES* qTables = &obj->qTables;
	/*WORD read = 2;
	BYTE num = qTables->num;
	int size = sizeof(struct QTable);
	BYTE qPrecision;
	BYTE b;
	int i;
	
	while(read < length){
		qTables->tables = realloc(qTables->tables, qTables->num*size);
		fread(&qTables->tables[num], 1, 1, file);
		read++;
		qPrecision = getHighNibble(qTables->tables[num].pindex);
		if(qPrecision == 0){
			for(i=0; i<64; i++){
				b = getByte(file);
				qTables->tables[num].qValues[i] = (WORD)b;	
				read++;	
			}
		}
		else if(qPrecision == 1){
			for(i=0; i<64; i++){
				qTables->tables[num].qValues[i] = getWord(file);
				read+=2;		
			}	
		}
		else{
			fprintf(stderr, "Unknown quantization table precision %i\n", qPrecision);
			return FALSE;	
		}
		num++;
		qTables->num = num;
	}
	return TRUE;*/
	WORD size = sizeof(struct QTable);
	WORD toAdd = length/size;
	BYTE oldNum = qTables->num;
	qTables->num += toAdd;
	qTables->tables = realloc(qTables->tables, qTables->num*size);
	fread(&qTables->tables[oldNum], toAdd*size, 1, file);
	return TRUE;
}

static void readDHT(JPGObj_T obj, FILE* file){
	WORD length = getWord(file);
	struct JPGHeader_HUFFMANTABLES* hTables = &obj->hTables;
	WORD read = 0;
	BYTE bindings;
	int i;
	while(read < length-2){
		if(hTables->read >= 4){
			fprintf(stderr, "Warning: Greater than four Huffman tables present\n");
			return;
		}
		/*Read in the index and bits[16] fields*/
		fread(&hTables->tables[hTables->read], 17, 1, file);
		/*Add up all of the values in bits to determine how many bindings
		 *there are in this Huffman table*/
		bindings=0;
		for(i=0; i<16; i++){
			bindings += hTables->tables[hTables->read].bits[i];	
		}
		hTables->tables[hTables->read].bindings = bindings;
		hTables->tables[hTables->read].hValues = (BYTE*)calloc(bindings, 1);
		fread(hTables->tables[hTables->read].hValues, bindings, 1, file);
		read += (17+bindings);
		hTables->read++;
	}
}

static void readSOF(JPGObj_T obj, FILE* file){
	WORD length;
	struct JPGHeader_SOF* sof = &(obj->SOF);
	memset(sof, 0, sizeof(struct JPGHeader_SOF));
	length = getWord(file);
	fseek(file, -2, SEEK_CUR);
	/*To prevent buffer overrun attacks*/
	if(length > sizeof(struct JPGHeader_SOF)){
		fprintf(stderr, "Warning: Start of Frame Header is too big\n");
		length = sizeof(struct JPGHeader_SOF);	
	}
	fread( sof, length, 1, file);	
	/*Words height, width*/
	sof->height = endianSwapW(sof->height);
	sof->width = endianSwapW(sof->width);
}

static void readSOS(JPGObj_T obj, FILE* file){
	WORD length;
	struct JPGHeader_SOS* sos = &(obj->SOS);
	memset(sos, 0, sizeof(struct JPGHeader_SOS));
	length = getWord(file);
	fseek(file, -2, SEEK_CUR);
	/*To prevent buffer overrun attacks*/
	if(length > sizeof(struct JPGHeader_SOS)){
		fprintf(stderr, "Warning: Start of Scan Header is too big: %i, %i\n", length, sizeof(struct JPGHeader_SOS));
		length = sizeof(struct JPGHeader_SOS);	
	}
	fread( sos, length, 1, file);	
}

static void readComment(JPGObj_T obj, FILE* file){
	WORD length = getWord(file);
	struct JPGHeader_COMMENT* comment = &(obj->comment);
	memset(comment, 0, sizeof(struct JPGHeader_COMMENT));
	comment->string = (char*)malloc(length - 1);
	fread(comment->string, 1, length-2, file);
	comment->string[length-2] = '\0';
}

static void handleUnknownMarker(WORD marker, FILE* file){
	BYTE length = getWord(file);
	fprintf(stderr, "Warning: Unknown header marker %x encountered\n", 
					marker);
	fseek(file, length - 2, SEEK_CUR);
}


void JPGObj_free(JPGObj_T jpgObj){
	int i;
	if(jpgObj->qTables.tables != NULL){
		free(jpgObj->qTables.tables);
	}
	for(i=0; i<4; i++){
		if(jpgObj->hTables.tables[i].hValues != NULL){
			free(jpgObj->hTables.tables[i].hValues);
			HTree_free(jpgObj->hTables.tables[i].hTree);
		}
	}
	if(jpgObj->comment.string != NULL){
		free(jpgObj->comment.string);
	}
	/*for(i=0; i<3; i++){
		if(jpgObj->coefficients[i] != NULL)
			free(jpgObj->coefficients[i]);	
	}*/
	free(jpgObj);	
}


JPGObj_T JPGObj_new(char* filename, int doDCT){
	JPGObj_T jpgObj;
	FILE* file = fopen (filename, "rb");
	WORD imageID;
	WORD marker;
	BYTE b;
	int i,j, sum;
	
	if(file == NULL){
		fprintf(stderr, "Error: Unable to open file %s\n", filename);
		return NULL;	
	}
	imageID = getWord(file);
	if(imageID != JPG_SOI){
		fprintf(stderr, "Error: This is not a jpeg file\n");
		return NULL;	
	}
	
	jpgObj = malloc(sizeof(struct JPGObj));
	assert(jpgObj != NULL);
	jpgObj->qTables.num = 0;
	jpgObj->qTables.tables = NULL;
	jpgObj->hTables.read = 0;
	jpgObj->grayscale = FALSE;
	
	for(i=0; i<4; i++){
		jpgObj->hTables.tables[i].hValues = NULL;	
		jpgObj->hTables.tables[i].bindings = 0;
	}
	
	while(TRUE){
	marker = getWord(file);
	
	if(jpgObj == NULL){
		fclose(file);
		return NULL;
		}
	
	if(marker == JFIF_MARKER){
		readJFIF(jpgObj, file);	
	}
	else if(marker == DQT_MARKER){
		if(!readDQT(jpgObj, file)){
			JPGObj_free(jpgObj);
			jpgObj = NULL;	
		}
	}
	else if(marker == DHT_MARKER){
		readDHT(jpgObj, file);
	}
	else if(marker == SOF_MARKER){
		readSOF(jpgObj, file);	
	}
	else if(marker == SOS_MARKER){
		readSOS(jpgObj, file);
		break;	
	}
	else if(marker == COMMENT_MARKER){
		readComment(jpgObj, file);	
	}
	else{
		handleUnknownMarker(marker, file);
	}
	
	fread(&b, 1, 1, file);
	if(b != 0xFF){
		fprintf(stderr, "Warning: Headers don't line up\n");	
	}
	while(b != 0xFF){
		if(fread(&b, 1, 1, file)==0){
			fprintf(stderr, "Error: EOF reached before finished processing fileheader\n");	
			JPGObj_free(jpgObj);
			fclose(file);
			return NULL;
		}
	}
	fseek(file, -1, SEEK_CUR);
	
	}
	
	printf("\n\nXdensity = %i\n", jpgObj->JFIFHeader.Xdensity);
	printf("Ydensity = %i\n", jpgObj->JFIFHeader.Ydensity);
	for(i=0; i<jpgObj->qTables.num; i++){
		printf("QTable %x\n", jpgObj->qTables.tables[i].pindex);
		for(j=0; j<64; j++){
			printf("%i ", jpgObj->qTables.tables[i].qValues[j]);
		}
		printf("\n\n");
	}
	printf("\n\n");
	for(i=0; i<4; i++){
		sum = 0;
		printf("Huffman Table %x:\n", jpgObj->hTables.tables[i].index);
		printf("Bits: {");
		for(j=0; j<16; j++){
			printf("%i,", jpgObj->hTables.tables[i].bits[j]);
			sum += jpgObj->hTables.tables[i].bits[j];
		}
		printf("}\nHuffman Byte Values: {");
	    for(j=0; j<sum; j++){
		 	printf("%x,", jpgObj->hTables.tables[i].hValues[j]);   
	    }
	    printf("}\n\n");
	}
	printf("width = %i\n", jpgObj->SOF.width);
	printf("height = %i\n", jpgObj->SOF.height);
	
	printf("\n(Component ID, HorizontalSample/VerticalSample, Quantization table number, DC Table/AC Table)\n");
	printf("(%i, %x, %i, %x)\n", jpgObj->SOF.c1ID, jpgObj->SOF.c1HVSample, jpgObj->SOF.c1QTable, jpgObj->SOS.c1HTable);
	printf("(%i, %x, %i, %x)\n", jpgObj->SOF.c2ID, jpgObj->SOF.c2HVSample, jpgObj->SOF.c2QTable, jpgObj->SOS.c2HTable);
	printf("(%i, %x, %i, %x)\n", jpgObj->SOF.c3ID, jpgObj->SOF.c3HVSample, jpgObj->SOF.c3QTable, jpgObj->SOS.c3HTable);
	
	jpgObj->doDCT = doDCT;
	if(!readData(jpgObj, file)){
		JPGObj_free(jpgObj);
		jpgObj = NULL;	
	}
	return jpgObj;
	
	/*Read in all of the components of the header first*/
}

static void DCT(JPGObj_T jpgObj){
	int i, j, k, l, p, x, y;
	int tempint;

	double last[] = {0.0, 0.0, 0.0}; /*Store previous values of Y, Cb, and Cr components
	*for differential DC component coding*/
	int xBlock = 0, yBlock = 0;
	double coefficient;
	
	double xFreq[64], yFreq[64];
	BOOL currentX = 0, currentY = 0;
	BOOL upRight = TRUE;
	
	Coefficients* coeff = jpgObj->coefficients[0];
	WORD* coefficients = coeff[0].values;
	
	printf("Begin drawing\n");
	
	if(jpgObj == NULL)
		return;
	
	/*Set up the zig zag pattern for decoding the coefficients*/
	xFreq[0] = 0; yFreq[0] = 0;
	for(i=1; i<64; i++){
		if(upRight){
			currentY--;
			currentX++;
			if(currentY < 0){
				upRight = FALSE;
				currentY = 0;
			}
			if(currentX > 7){
				upRight = FALSE;
				currentX = 7;
				currentY += 2;	
			}
		}
		else{
			currentY++;
			currentX--;
			if(i<36){
				if(currentX < 0){
					currentX = 0;
					upRight = TRUE;	
				}
			}
			else{
				if(currentY > 7){
					currentY = 7;
					currentX += 2;
					upRight = TRUE;	
				}
			}
		}
		yFreq[i] = currentY;
		xFreq[i] = currentX;
	}

	/*printf("\n\nZIGZAG SCAN:\n\n");
	for(i=0; i<64; i++){
		printf("%i: (%i, %i)\n",i, iFreq[i]+1, jFreq[i]+1);
	}*/

	/*for(i=0; i<64; i++){
		printf("%f ", (float)(short int)coefficients[i]);	
	}*/
	
	/*Perform the inverse DCT on every pixel for every component*/
	printf("Starting DCT\n");
	for(k=0; k<jpgObj->SOS.components; k++){
		tempint = jpgObj->components[k].hSample*jpgObj->components[k].vSample;
		coeff = jpgObj->coefficients[k];
		for(l=0; l < jpgObj->mcuX * jpgObj->mcuY * tempint; l++){
            coefficients = coeff[l].values;
            coefficients[0] = coefficients[0] + last[k];
            last[k] = coefficients[0];
			for(x=0; x<8; x++){
			for(y=0; y<8; y++){
			jpgObj->pixels[k][l][x][y] = 0.0;
			for(p=0; p<64; p++){
				coefficient = (double)((short int)(coefficients[p]));	
				jpgObj->pixels[k][l][x][y] += ((p==0)?0.125:0.25)* coefficient*
								cos( (2.0*(double)x + 1.0)*xFreq[p]*PI/16.0 )*
								cos( (2.0*(double)y + 1.0)*yFreq[p]*PI/16.0 );
			}
            jpgObj->pixels[k][l][x][y] += 128.0;
        	}
     	}
 		}
    }
    printf("Finished DCT");
}

static BOOL readData(JPGObj_T jpgObj, FILE* file){
	int i, j, k;
	struct JPGHeader_QTABLES qTables = jpgObj->qTables;
	struct JPGHeader_HUFFMANTABLES hTables = jpgObj->hTables;
	struct JPGHeader_SOF SOF = jpgObj->SOF;
	struct JPGHeader_SOS SOS = jpgObj->SOS;
	BYTE temp;
	int processedMCUs = 0;
	int greatestHSample = 1, greatestVSample = 1;
	int index[] = {0, 0, 0};/*Indexes for coefficients*/
	BYTE bitPos = 0;
	WORD readEOI;
	
	/*Step 1: Set up the Huffman Trees based on the info reported
	 *by the header 
	 *NOTE: It is an unchecked runtime error for the Huffman information
	 *in the header not to form a valid Huffman Table.  This will not crash
	 *the program, since the recursion in HTree.c has stopping states, but
	 *decoding will not work at all*/
	 for(i=0; i<4; i++){
		if(hTables.tables[i].bindings > 0){
		 hTables.tables[i].hTree = HTree_new(
		 hTables.tables[i].bits, hTables.tables[i].hValues);
	 	}
	 }
	 
	 /*Step 2: Set up objects that represent the different components of
	  *the scan.  If the image is grayscale, it will only have a Y component.
	  *Otherwise, it will have Y, Cb, and Cr components*/
	if(SOS.components == 1){
		jpgObj->grayscale = TRUE;	
	}
	else if(SOS.components == 3){
		jpgObj->grayscale = FALSE;
	}
	else{
		fprintf(stderr, "Error: Unknown number of components (%i)\n",
						SOS.components); 
		return FALSE;	
	}
	
	/*NOTE: The components should be in scan order in the header*/
	jpgObj->components[0].hSample = getHighNibble(SOF.c1HVSample);
	jpgObj->components[0].vSample = getLowNibble(SOF.c1HVSample);
	temp = getHighNibble(SOS.c1HTable);
	for(i=0; i<4; i++){
		if(hTables.tables[i].index == temp){
			jpgObj->components[0].dcTree = hTables.tables[i].hTree;
		}
	}
	temp = getLowNibble(SOS.c1HTable) + 0x10;
	for(i=0; i<4; i++){
		if(hTables.tables[i].index == temp){
			jpgObj->components[0].acTree = hTables.tables[i].hTree;
		}
	}
	for(i=0; i<qTables.num; i++){
		if(SOF.c1QTable == getLowNibble(qTables.tables[i].pindex)){
			jpgObj->components[0].qTable = &qTables.tables[i];	
		}	
	}
	if(jpgObj->components[0].hSample >  greatestHSample)
		greatestHSample = jpgObj->components[0].hSample;
	if(jpgObj->components[0].vSample >  greatestVSample)
		greatestVSample = jpgObj->components[0].vSample;
			
		
	jpgObj->components[1].hSample = getHighNibble(SOF.c2HVSample);
	jpgObj->components[1].vSample = getLowNibble(SOF.c2HVSample);
	temp = getHighNibble(SOS.c2HTable);
	for(i=0; i<4; i++){
		if(hTables.tables[i].index == temp){
			jpgObj->components[1].dcTree = hTables.tables[i].hTree;
		}
	}
	temp = getLowNibble(SOS.c2HTable) + 0x10;
	for(i=0; i<4; i++){
		if(hTables.tables[i].index == temp){
			jpgObj->components[1].acTree = hTables.tables[i].hTree;
		}
	}
	for(i=0; i<qTables.num; i++){
		if(SOF.c2QTable == getLowNibble(qTables.tables[i].pindex)){
			jpgObj->components[1].qTable = &qTables.tables[i];	
		}	
	}
	if(jpgObj->components[1].hSample >  greatestHSample)
		greatestHSample = jpgObj->components[1].hSample;
	if(jpgObj->components[1].vSample >  greatestVSample)
		greatestVSample = jpgObj->components[1].vSample;

	jpgObj->components[2].hSample = getHighNibble(SOF.c3HVSample);
	jpgObj->components[2].vSample = getLowNibble(SOF.c3HVSample);
	temp = getHighNibble(SOS.c3HTable);
	for(i=0; i<4; i++){
		if(hTables.tables[i].index == temp){
			jpgObj->components[2].dcTree = hTables.tables[i].hTree;
		}
	}
	temp = getLowNibble(SOS.c3HTable) + 0x10;
	for(i=0; i<4; i++){
		if(hTables.tables[i].index == temp){
			jpgObj->components[2].acTree = hTables.tables[i].hTree;
		}
	}
	for(i=0; i<qTables.num; i++){
		if(SOF.c3QTable == getLowNibble(qTables.tables[i].pindex)){
			jpgObj->components[2].qTable = &qTables.tables[i];
		}	
	}
	if(jpgObj->components[2].hSample >  greatestHSample)
		greatestHSample = jpgObj->components[2].hSample;
	if(jpgObj->components[2].vSample >  greatestVSample)
		greatestVSample = jpgObj->components[2].vSample;
		
	jpgObj->mcuX = SOF.width / (8*greatestHSample);
	jpgObj->mcuX += (SOF.width % (8*greatestHSample) > 0)?1:0;
	jpgObj->mcuY = SOF.height / (8*greatestVSample);
	jpgObj->mcuY += (SOF.height % (8*greatestVSample) > 0)?1:0;
	jpgObj->greatestHSample = greatestHSample;
	jpgObj->greatestVSample = greatestVSample;
	printf("mcuX = %i, mcuY = %i\n", jpgObj->mcuX, jpgObj->mcuY);
	
	/*Step 3: Start decoding the data section of the file by figuring
	 *out which component is being read and sending the data stream to 
	 *the Huffman Tree file to be processed*/
	bitPos = 8;
	/*temp will store the current byte that is being processed in the data section*/
	index[0] = 0; index[1] = 0; index[2] = 0;
	while(!feof(file) && processedMCUs < (jpgObj->mcuX * jpgObj->mcuY)){
		for(i=0; i<SOS.components; i++){
			for(j=0; j<jpgObj->components[i].hSample; j++){
				for(k=0; k<jpgObj->components[i].vSample; k++){
					HTree_decode(jpgObj->components[i].dcTree, jpgObj->components[i].acTree,
								 jpgObj->components[i].qTable->qValues, jpgObj->coefficients[i], 
								 index[i], file, &temp, &bitPos);
					index[i]++;
				}	
			}
		}
		processedMCUs++;
	}
	readEOI = getWord(file);
	if(readEOI != EOI){
		fprintf(stderr, "Warning: extra bytes in image unprocessed or EOI missing\n");
		printf("%x\n", readEOI);
		while(!feof(file)){
			fprintf(stderr, "Extra byte %x\n", getByte(file));	
		}
	}
	if(jpgObj->doDCT)
       DCT(jpgObj);
	return TRUE;
}
    
void JPGObj_draw(JPGObj_T jpgObj, HDC* hdc, PAINTSTRUCT* ps){
    /*Loop through the MCUs, and within the MCUs, loop through the pixels*/
   	BYTE R, G, B;
	double Y, Cb, Cr;
	int index[] = {0, 0, 0};
	int i,j,x,y;

    for(i=0; i<jpgObj->mcuY; i++){
	    for(j=0; j<jpgObj->mcuX; j++){
			for(x=0; x<8; x++){
				for(y=0; y<8; y++){
					Y = jpgObj->pixels[0][index[0]][x][y];
					Cb = jpgObj->pixels[1][index[1]][x][y];
					Cr = jpgObj->pixels[2][index[2]][x][y];
					toRGB(Y, Cb, Cr, &R, &G, &B);
					/*SetPixel(*hdc, j*16+x, i*16+y, RGB(R, G, B));*/
				}	
			}
			index[0]++;
			for(x=0; x<8; x++){
				for(y=0; y<8; y++){
					Y = jpgObj->pixels[0][index[0]][x][y];
					Cb = jpgObj->pixels[1][index[1]][x][y];
					Cr = jpgObj->pixels[2][index[2]][x][y];
					toRGB(Y, Cb, Cr, &R, &G, &B);
					/*SetPixel(*hdc, j*16+8+x, i*16+y, RGB(R, G, B));*/
				}	
			}
			index[0]++;
			
			for(x=0; x<8; x++){
				for(y=0; y<8; y++){
					Y = jpgObj->pixels[0][index[0]][x][y];
					Cb = jpgObj->pixels[1][index[1]][x][y];
					Cr = jpgObj->pixels[2][index[2]][x][y];
					toRGB(Y, Cb, Cr, &R, &G, &B);
					/*SetPixel(*hdc, j*16+x, i*16+8+y, RGB(R, G, B));*/
				}	
			}
			index[0]++;
			
			for(x=0; x<8; x++){
				for(y=0; y<8; y++){
					Y = jpgObj->pixels[0][index[0]][x][y];
					Cb = jpgObj->pixels[1][index[1]][x][y];
					Cr = jpgObj->pixels[2][index[2]][x][y];
					toRGB(Y, Cb, Cr, &R, &G, &B);
					/*SetPixel(*hdc, j*16+x+8, i*16+y+8, RGB(R, G, B));*/
				}	
			}
			index[0]++;
			index[1]++;
			index[2]++;
	    }
    }
    
    /*for(i=0; i<8; i++){
		for(j=0; j<8; j++){
			toRGB(jpgObj->pixels[0][0][i][j],0, 0, &R, &G, &B);
			printf("%i, %i, %i\n", R, G, B);
			SetPixel(*hdc, j, i, RGB(G, R, B));	
		}
	}*/
}




