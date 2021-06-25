/*This file was supposed to manually open up a file containing raw 2-byte
little endian sound samples (16000hz) and send it to the buffer for outputting,
but there were issues with linking the library, so I took the temporary
approach of just calling "play" from the linux console*/

#include "play.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/file.h>
#include <sys/errno.h>
#include <sys/param.h>

#include <s2types.h>
#include <ad.h>
#include <err.h>

#define SAMPLE_RATE 16000

struct SoundEffect {
	unsigned long length;
	int16* contents;
};

/*Use the function below to convert two chars
*into an unsigned int
*NOTE: Data comes in little endian*/
int16 getInt(char* array, int pos) {
	int16 toReturn = 0;
	int16 i1 = (int16)array[pos];
	int16 i2 = (int16)array[pos + 1];
	toReturn = i1 | (i2 << 8);
	return toReturn;
}


int main(int argc, char** argv) {
	char* filename = "alarm.raw";
	char* rawData;
	unsigned long filelength;
	unsigned long i;
	int32 err;
	struct SoundEffect* sound = malloc(sizeof(struct SoundEffect));
	FILE* file = fopen(filename, "rb");
	ad_play_t* ad;
	ad_rec_t* adrec = ad_open();

	if (file == NULL) {
		fprintf(stderr, "Error: unable to find file %s\n", filename); 
	}
	
	fseek(file, 0, SEEK_END);
	filelength = ftell(file);
	fseek(file, 0, SEEK_SET);
	rawData = (char*)malloc(filelength);
	fread(rawData, 1, filelength, file);	

	sound->length = filelength / 2;
	sound->contents = (int16*)calloc(sound->length, 2);
	for (i = 0; i < sound->length; i++) {
		sound->contents[i] = getInt(rawData, 2*i);
		printf("%i ", sound->contents[i]);
	}
	free(rawData);
	fclose(file);

	ad = ad_open_play_sps(SAMPLE_RATE);
	if(ad_start_play(ad) < 0) {
		fprintf(stderr, "Error: Unable to open device for playback\n");
		return 1;
	}
	err = ad_write(ad, sound->contents, sound->length);
	printf("%i", err);

	ad_stop_play(ad);
	ad_close_play(ad);

	free(sound->contents);
	free(sound);
	return 0;
}
