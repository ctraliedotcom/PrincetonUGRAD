/*Programmer: Chris Tralie (ctralie@princeton.edu)*/
/*Purpose: To create a class that can convert information from
*a JPEG file's Huffman Table into an actual binary Huffman Tree, 
*which can then be used to decode compressed binary data.  Note
*that the run-length coding scheme is also implemented in the decode
*data stream section*/

#include "HTree.h"
#include "bitOps.h"
#include "JPGObj.h"

struct HTree{
	HTree_T left;
	HTree_T right;
	BYTE value;	
	BOOL leafNode;
};

static HTree_T newNode(){
	HTree_T oHTree;
	oHTree = (HTree_T)malloc(sizeof(struct HTree));
	oHTree->left = NULL;
	oHTree->right = NULL;
	oHTree->leafNode = FALSE;
	return oHTree;
}

static void cleanUp(HTree_T node){
	if(node == NULL)
		return;
	cleanUp(node->left);
	cleanUp(node->right);
	if(node->left == NULL && node->right == NULL){
		if(!node->leafNode)
			HTree_free(node);	
	}
}

/*A recursive function to fill in the values at a particular level of
 *the Huffman Tree*/
void setValues(HTree_T node, BYTE currentLevel, BYTE targetLevel,
			   BYTE* set, BYTE num, BYTE* hValues, int offset){
	if(currentLevel == targetLevel){
		if(*set < num){
			node->left = newNode();
			node->left->leafNode = TRUE;
			node->left->value = hValues[offset + *set];
			*set = *set+1;
		}
		else{
			if(currentLevel < 15)
				node->left = newNode();	
		}
		if(*set < num){
			node->right = newNode();
			node->right->leafNode = TRUE;
			node->right->value = hValues[offset + *set];
			*set = *set+1;
		}
		else{
			if(currentLevel < 15)
				node->right = newNode();
		}
	}
	else{
		if(!node->left->leafNode){
			setValues(node->left, currentLevel +1, targetLevel,
			set, num, hValues, offset);
		}
		if(!node->right->leafNode){
			setValues(node->right, currentLevel +1, targetLevel,
			set, num, hValues, offset);
		}	
	}
}

HTree_T HTree_new(BYTE* bits, BYTE* hValues){
	HTree_T rootNode;
	BYTE i;
	BYTE set;
	int offset = 0;
	rootNode = newNode();
	for(i=0; i<16; i++){
		set = 0;
		setValues(rootNode, 0, i, &set, bits[i], hValues, offset);
		offset += bits[i];
	}
	return rootNode;
}

void HTree_free(HTree_T oHTree){
	if(oHTree == NULL) return;
	HTree_free(oHTree->left);
	HTree_free(oHTree->right);
	free(oHTree);
}

/*Helper for run-length portion*/
/*i represents bit position in current byte (b)*/
static WORD getFromCategory(BYTE category, BYTE* b, BYTE* i, FILE* file){
	BOOL invert = FALSE;
	BYTE pos = 16 - category;
	WORD decoded = 0x0000;
	if(getBit(*b, *i) == 0)
		invert = TRUE;
	while(pos < 16){
		if(*i > 7){
			*i = 0;
			*b = getByteF(file);
			/*printBinary(*b);*/	
		}
		if(!invert)
			setBitW(&decoded, pos, getBit(*b, *i));
		else
			setBitW(&decoded, pos, (getBit(*b, *i) == 1)?0:1);
		*i = *i+1;
		pos++;
	}
	if(invert)
		return -decoded;
	return decoded;
}

/*An example of Huffman decoding*/
void HTree_decodeTest(HTree_T oHTree, BYTE* data, int length, BYTE* decoded){
	int i, pos;
	BYTE j;
	HTree_T node = oHTree;
	BYTE bit;
	pos = 0;

	for(i=0; i<length; i++){
		for(j=0; j<8; j++){
			bit = getBit(data[i], j);
			if(bit == 0){
				printf("0");
				node = node->left;	
			}
			else{
				printf("1");
				node = node->right;
			}
			if(node->leafNode){
				decoded[pos] = node->value;
				node = oHTree;
				pos++;
			}
		}
	}	
}

void HTree_decode(HTree_T dcTable, HTree_T acTable, BYTE* qTable, 
				  Coefficients* coefficients, int index, FILE* file,
				  BYTE* b, BYTE* bitPos){
	WORD* uncompressed = coefficients[index].values;
	HTree_T node = dcTable;
	BYTE bit, value;
	BOOL decodingCategory = FALSE;
	WORD temp;
	int i, j;
	
	
	for(i=0; i<64; i++){
		uncompressed[i] = 0;	
	}
	i = 0;
	
	while(i < 64 && !feof(file)){
		/*printf("i = %i\n", i);*/
		if(*bitPos > 7){
			*bitPos = 0;
			*b = getByteF(file);
			/*printBinary(*b);*/
		}
		if(decodingCategory){
			if(value == EOB){
				if(i > 0){
					/*printf("EOB encountered at i=%i\n", i);*/
					break;	
				}
				else{
					uncompressed[0] = 0;	
				}	
			}
			else if(i == 0){/*Decoding CD coefficient*/
				temp = getFromCategory(value, b, bitPos, file);
				uncompressed[0] = temp;
			}
			else{/*Decoding AC coefficient*/
				/*Fill in previous zeroes (this is the run-length portion)*/
				for(j = 0; j < getHighNibble(value); j++){
					if(i > 63){
						fprintf(stderr, "Error: Greater than 64 DCT coefficients encountered\n");	
						break;
					}
					uncompressed[i] = 0;
					i++;
				}
				if(i < 64){
				temp = getFromCategory(getLowNibble(value), b, bitPos, file);
				uncompressed[i] = temp;
				}				
			}
			i++;
			decodingCategory = FALSE;
			node = acTable;
		}
		else{
			if(node->leafNode){
				value = node->value;
				decodingCategory = TRUE;
			}
			else{
				bit = getBit(*b, *bitPos);
				*bitPos = *bitPos + 1;
				if(bit == 0)
					node = node->left;
				else
					node = node->right;	
			} 
		}
		if(feof(file)){
			fprintf(stderr, "Warning: end of file reached before finished decoding data section\n");	
		}
	}
	
	/*Now dequantize the values*/
	for(i=0; i<64; i++){
		uncompressed[i] = uncompressed[i] * qTable[i];
		/*printf("%i ", (int)((short int)uncompressed[i]));*/
	} 
}
