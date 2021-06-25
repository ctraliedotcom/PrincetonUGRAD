/*Programmer: Chris Tralie (adapted from Perry Cook's wavio.h class for communicating
*with wave files
*Purpose: To compose a song using beating cosine waves*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <map.h>
#include <iterator.h>
#define TWO_PI 6.28318530718
#define SRATE 44100
#include "waveio.h"
#define PEAKMAX 30000

//Used as a comparator for the map
struct Compare {
	bool operator()(const double f1, const double f2) const {
		return (f1 < f2);
	}
};

/*A class to encapsulate phasors*/
struct Phasor {
	Phasor(double f);
	~Phasor();
	double freq;
	double phaseX;
	double phaseY;
	void addPhase(double gain, double phase);
	double getAmp();
	double getPhase();
};

Phasor::Phasor(double f) {
	freq = f;
	phaseX = 0;
	phaseY = 0;
}

Phasor::~Phasor() {}

void Phasor::addPhase(double gain, double phase) {
	phaseX += gain*cos(phase);
	phaseY += gain*sin(phase);
	//printf("(%f, %f)\n", phaseX, phaseY);
}

double Phasor::getAmp() {
	//printf("%f\n\n", phaseX*phaseX + phaseY*phaseY);
	return sqrt(phaseX*phaseX + phaseY*phaseY);
}

double Phasor::getPhase() {
	//KEEP IN MIND THAT THE PRINCIPLE VALUE SET OF TAN IS FROM -PI/2 to PI/2
	//(I had a bug where I forgot to do that)
	if (phaseX == 0.0 && phaseY > 0)
		return TWO_PI / 4.0;
	else if (phaseX == 0.0 && phaseY < 0)
		return -TWO_PI / 4.0;
	double angle = atan(phaseY / phaseX);
	//Quadrants 1 and 4, atan returns the correct angle
	if (phaseX > 0)
		return angle;
	//Quadrant 3, need to add PI
	if (phaseY < 0)
		return angle + TWO_PI / 2;
	//Quadrant 2, need to add PI
	return angle + TWO_PI / 2;
}

void checkUsage(int argc, char** argv) {
	if (argc != 5) {
		fprintf(stderr, "Usage: beats <songfile.txt> <real: songlength> <int: numbeats> <outputfile.wav>\n");
		exit(0);
	}
}

//Use the logarithmic scale to 
double toFreq(int note) {
	double base = pow(2.0, 1.0 / 12.0);
	return 440.0 * pow(base, (double)note);
}

//Go through this note and calculate all of the frequencies needed.  If any of the frequencies
//already exist in the phasor table, simply add the appropriate phasor from this note
void addPhasors(map<double, struct Phasor*, Compare>* freqTable, double basefreq, double time, double songLength, int numFreqs) {
	int i, j;
	double freq = basefreq - (0.5 / songLength * numFreqs);//Space the intefering frequencies evenly
	//around the base frequency in question
	double phase;
	for (i = 0; i < numFreqs; i++) {
		phase = -time * freq * TWO_PI;
		if (freqTable->find(freq) == freqTable->end()) {
			(*freqTable)[freq] = new Phasor(freq);
		}
		(*freqTable)[freq]->addPhase(1.0, phase);
		freq += 1.0 / songLength;
	}
}

//After all of the unique frequencies have been identified with their appropriate 
//amplitudes and phases, add each one to the final output signal
void synthesizePhasors(map<double, struct Phasor*, Compare>* freqTable, double* signal, int buflen) {
	map<double, struct Phasor*, Compare>::iterator iter = freqTable->begin();
	struct Phasor* currentPhasor;
	double freq;
	double phase;
	double amp;
	printf("There are %i cosines:\n", freqTable->size());
	printf("Frequency (hz), Amplitude, Phase (radians)\n");
	while (iter != freqTable->end()) {
		currentPhasor = (*freqTable)[(*iter).first];
		freq = currentPhasor->freq;
		phase = currentPhasor->getPhase();
		amp = currentPhasor->getAmp();
		delete currentPhasor;
		printf("%f, %f, %f\n", freq, amp, phase);
		for (int i = 0; i < buflen; i++) {
			signal[i] += amp*cos(TWO_PI * freq * ((double)i)/SRATE + phase);
		}
		iter++;
	}
}

//Get the max value in a; used to prevent clipping
double getMax(double* a, int len) {
	int i = 0;
	double toReturn = 0.0;
	for (i = 0; i < len; i++) {
		double value = a[i];
		if (value > toReturn)
			toReturn = value;
		if (-value > toReturn)
			toReturn = -value;
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
	int numNotes, buflen, FreqsPerNote;
	map<double, struct Phasor*, Compare> freqTable;

	checkUsage(argc, argv);
	songLength = atof(argv[2]);
	FreqsPerNote = atoi(argv[3]);
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
		return 1;
	}
	notes = (int*)calloc(numNotes, sizeof(double));
	rhythm = (int*)calloc(numNotes, sizeof(int));
	times = (double*)calloc(numNotes, sizeof(double));
	//Read in input file data
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
		addPhasors(&freqTable, toFreq(notes[i]), times[i], songLength, FreqsPerNote);
	}
	synthesizePhasors(&freqTable, signal, buflen);
	maxAmp = getMax(signal, buflen);
    fillheader(&hdr,SRATE);
    fileOut = opensoundout(argv[4],&hdr);
    total = 0;
	//printf("%f\n", PEAKMAX / maxAmp);
	//Write the data to the file in 256 sample chunks
	for (i=0; i<buflen/256; i++) {
        for (k=0; k<256; k++,total++) {
			if (total >= buflen)
				buf[k] = 0;
			else
				buf[k] = (int)((PEAKMAX / maxAmp) * signal[total]);
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
