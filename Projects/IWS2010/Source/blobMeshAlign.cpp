#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <stdio.h>
#include <limits>

#include "BlobMeshAligner.h"

using namespace std;

//Used as a comparator for the maps
struct Compare {
	bool operator()(const int f1, const int f2) const {
		return (f1 < f2);
	}
};

double inf()
{
  return atof("infinity");
}

map<int, double, Compare> meshToZGround;

void loadBlobsGroundPos(char* filename) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		fprintf(stderr, "ERROR opening zcoords mesh ground file\n");
		return;
	}
	while (!feof(f)) {
		int num;
		double zpos;
		fscanf(f, "%i %lf", &num, &zpos);
		meshToZGround[num] = zpos;
	}
}

//Get the number category
int getNum(string str) {
	int parenthIndex = str.find_first_of("_");
	string sub = str.substr(0, parenthIndex);
	return atoi(sub.data());
}

double getZTrans(string blobname) {
	const char* data = blobname.data();
	for (int i = blobname.size() - 5; i >= 0; i--) {
		if (data[i] == '_') {
			string str = blobname.substr(i+1, blobname.size() - 5 - i);
			return atof(str.data());
		}
	}
	return 0.0;
}


int main(int argc, char** argv) {
 	if (argc < 7) {
		fprintf(stderr, "Usage: blobMeshAlign <mesh> <blob> <aligned blob out> <3D?> <groundpos file> <eps>");
		return 1;
	}
	bool DOF3 = (bool)atoi(argv[4]);
	loadBlobsGroundPos(argv[5]);
	double eps = atof(argv[6]);
	int num = getNum(string(argv[1]));
	BlobMeshAligner aligner(argv[1], argv[2], argv[3], DOF3, 100);

	printf("\n\n num = %i \n\n", num);

	printf("\n\n\nBlob Z-Coordinate: %.3f\nMesh Z-Coordinate:%.3f\n\n\n", getZTrans(string(argv[2])), meshToZGround[num]);

	aligner.translatePointsZ(getZTrans(argv[2]), meshToZGround[num]);
	aligner.doAlign(true);
	aligner.getPointsToMeshFrac(eps);
	printf("\n\n\nALIGNMENT SCORE: %.3f\n\n\n", aligner.getPointsToMeshFrac(eps));
	char command[512];
	sprintf(command, "simpleModelFittingViewer %s %s", argv[1], argv[3]);
	system((const char*)command);
	return 0;
}
