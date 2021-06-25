#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define TWO_PI 6.28318530718
#define SRATE 44100
#include "waveio.h"
#define PEAKMAX 30000

void checkUsage(int argc, char** argv) {
	if (argc != 5) {
		fprintf(stderr, "Usage: beats <songfile.txt> <real: songlength> <int: numbeats> <outputfile.wav>\n");
		exit(0);
	}
}

double toFreq(int note) {
	double base = pow(2.0, 1.0 / 12.0);
	return 440.0 * pow(base, (double)note);
}

void addFreq(double* signal, int buflen, double basefreq, double time, double songLength, int numFreqs) {
	int i, j;
	double freq = basefreq - (0.5 / songLength * numFreqs);//Space the intefering frequencies evenly
	//around the base frequency in question
	double phase;
	for (i = 0; i < numFreqs; i++) {
		phase = -time * freq;
		printf("Frequency: %f hz, Phase: %f (%f sec)\n", freq, phase, time);
		for (j = 0; j < buflen; j++) {
			signal[j] += sin(TWO_PI * freq * ((double)j)/SRATE + phase * TWO_PI);
		}
		freq += 1.0 / songLength;
	}
}

double getMax(double* a, int len) {
	int i = 0;
	double toReturn = 0.0;
	for (i = 0; i < len; i++) {
		if (abs(a[i]) > toReturn)
			toReturn = abs(a[i]);
	}
	return toReturn;
}

int main(int argc, char** argv) {
    long i,j,k,total,freq;
    double sixteenthLength, maxAmp, songLength;
    short buf[256];
    FILE* fileOut;
	FILE* dataIn;
    struct soundhdr hdr;
	int* notes;
	int* rhythm;
	double* times;//Time to hit each note
	double* signal;
	int numNotes, buflen, numPerFreq;

	checkUsage(argc, argv);
	songLength = atof(argv[2]);
	numPerFreq = atoi(argv[3]);
	buflen = (int)songLength * SRATE;
	signal = (double*)calloc(buflen, sizeof(double));
	dataIn = fopen(argv[1], "r");
	if (dataIn == NULL) {
		fprintf(stderr, "ERROR opening input file %s\n", argv[1]);
		return 1;
	}
	fscanf(dataIn, "%i", &numNotes);
	if (numNotes < 1) {
		fprintf(stderr, "ERROR: Should have at least one note\n");
		return;
	}
	notes = (int*)calloc(numNotes, sizeof(double));
	rhythm = (int*)calloc(numNotes, sizeof(int));
	times = (double*)calloc(numNotes, sizeof(double));
	for (i = 0; i < numNotes; i++) {
		fscanf(dataIn, "%i %i\n", &notes[i], &rhythm[i]);
		//printf("(%i, %i)\n", notes[i], rhythm[i]);
	}
	//Let 1 be the length of a sixteenth note
	total = 0;
	for (i = 0; i < numNotes; i++) {
		total += rhythm[i];
	}
	sixteenthLength = songLength / (double)total;
	times[0] = 0.0;
	for (i = 1; i < numNotes; i++) {
		times[i] = times[i - 1] + rhythm[i - 1]*sixteenthLength;
	}
	for (i = 0; i < numNotes; i++) {
		addFreq(signal, buflen, toFreq(notes[i]), times[i], songLength, numPerFreq);
	}
	maxAmp = getMax(signal, buflen);
	fillheader(&hdr,SRATE);
	fileOut = opensoundout(argv[4],&hdr);
	total = 0;
	//printf("%f\n", PEAKMAX / maxAmp);
	for (i=0; i<buflen/256; i++) {
        for (k=0; k<256; k++,total++) {
			if (total >= buflen)
				buf[k] = 0;
			else
				buf[k] = (int)(PEAKMAX / maxAmp) * signal[total];
        }
        fwrite(buf,256,2,fileOut);
    }
    closesoundout(fileOut,total);
	free(notes);
	free(rhythm);
	free(times);
	free(signal);
    return 0;
}
