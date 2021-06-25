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

//Extract the last component of the blob name, which should be *_ztrans.xyz
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

//Create the arff file header and return the file handle
FILE* startArff(char* file, vector<string>& meshnames, vector<string>& blobnames) {
	FILE* fout = fopen(file, "w");
	if (fout == NULL) {
		fprintf(stderr, "ERROR creating arff file %s\n", file);
		return NULL;
	}
	fprintf(fout, "@relation markers\n");
	//Let the user know how many blobs and how many mesh models there are
	printf("%i %i\n", (int)meshnames.size(), (int)blobnames.size());
	//Print out the name of each mesh in the order against which each blob will be
	//tested.  Each mesh will hold a value between 0 and 1 for the blob, which is
	//the ratio of the aligned points which are within a certain cutoff distance
	//epsilon of the mesh
	for (size_t i = 0; i < meshnames.size(); i++) {
		fprintf(fout, "@attribute %s real\n", meshnames[i].data());
	}
	//Put all of the possible blob numbers from the training set into the "type" category
	set<int> categories;
	for (size_t i = 0; i < blobnames.size(); i++) {
		categories.insert(getNum(blobnames[i]));
	}
	fprintf(fout, "@attribute type { ");
	set<int>::iterator iter = categories.begin();
	while (iter != categories.end()) {
		fprintf(fout, "%i, ", *iter);
		iter++;
	}
	fprintf(fout, " }\n@data\n");
	return fout;
}

int main(int argc, char** argv) {
	if (argc < 9) {
		fprintf(stderr, "Usage: BatchAlign <mesh directory> <blob directory> <results directory> <arff file> <3DOF?> <groundpos file> <eps> <maxpoints>");
		return 1;
	}
	char* meshdir = argv[1];
	char* blobdir = argv[2];
	char* resultsdir = argv[3];
	char* arff_file = argv[4];
	bool DOF3 = (bool)atoi(argv[5]);
	loadBlobsGroundPos(argv[6]);
	double eps = atof(argv[7]);
	int max_points = atoi(argv[8]);
	char meshindexname[256];
	char meshname[256];
	char blobname[256];

	//Get the list of meshes to test against by looking in the "index.txt" file
	//within the meshes directory
	sprintf(meshindexname, "%s/index.txt", meshdir);
	FILE* fin = fopen(meshindexname, "r");
	if (fin == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s to read meshes\n", meshindexname);
		return 1;
	}
	vector<string> meshes;//The full path to the mesh, including the mesh directory (e.g. meshes/blob.off)
	vector<string> meshnames;//The mesh name by itself (e.g. model.off)
	while (!feof(fin)) {
		char mesh[256];
		fscanf(fin, "%256s", mesh);
		sprintf(meshname, "%s/%s", meshdir, mesh);
		string str(meshname);
		meshes.push_back(str);
		string str2(mesh);
		meshnames.push_back(str2);
	}
	//Get the list of the blobs to test by looking in the "index.txt" file
	char blobindexname[256];
	sprintf(blobindexname, "%s/index.txt", blobdir);
	fclose(fin);
	fin = fopen(blobindexname, "r");
	if (fin == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s to read blobs\n", blobindexname);
		return 1;
	}
	vector<string> blobs;//The full path to the blob (e.g. blobs/blob.xyz)
	vector<string> blobnames;//The blob name by itself (e.g. blob.xyz)
	while (!feof(fin)) {
		char blob[256];
		fscanf(fin, "%256s", blob);
		sprintf(blobname, "%s/%s", blobdir, blob);
		string str(blobname);
		blobs.push_back(str);
		string str2(blob);
		blobnames.push_back(str2);
	}
	fclose(fin);

	//Based on the meshes and blobs in the respective index.txt files, create
	//the ARFF header
	FILE* arffHandle = startArff(arff_file, meshnames, blobnames);
	for (size_t i = 0; i < blobs.size(); i++) {
		for (size_t j = 0; j < meshes.size(); j++) {
			//Since this normally takes a very long time, print a '.' every time a new
			//score has been computed for aligning a blob to a mesh.  There will be
			//(# meshes) * (# blobs) total alignments done, so there will be that number
			//of periods outputted.
			printf(".");
			BlobMeshAligner* align;
			string outname = string(resultsdir) + "/" + blobnames[i] + "_" + (DOF3?"_3D_":"") + meshnames[j] + ".xyz";
			FILE* outfile = fopen(outname.data(), "r");
			int meshnum = getNum(meshnames[j]);
			if (outfile == NULL) {
				//The point set has not been aligned to this blob yet
				align = new BlobMeshAligner((char*)meshes[j].data(), 
											(char*)blobs[i].data(), 
											(char*)outname.data(),
											DOF3, max_points);
				if (DOF3) {
					align->translatePointsZ(getZTrans(blobnames[i]), meshToZGround[meshnum]);
				}
				align->doAlign(true);//Save the result for future tests by passing "true"
			}
			else {
				//The point set has already been aligned to this blob so don't re-do the work
				fclose(outfile);
				align = new BlobMeshAligner((char*)meshes[j].data(),
											(char*)outname.data(),
											(char*)outname.data(),
											DOF3, max_points);
				//The only thing that needs to be done is the initialization of the mesh
				//search tree, since the alignment hasn't been done from scratch this time
				align->initMeshTree();
			}
			fprintf(arffHandle, "%.3lf, ", align->getPointsToMeshFrac(eps));
			delete align;
			fflush(arffHandle);
		}
		//Now say what class it is
		fprintf(arffHandle, "%i", getNum(blobnames[i]));
		printf("Finished blob %i of %i\n", (int)(i+1), (int)blobs.size());
		fprintf(arffHandle, "\n");
		fflush(arffHandle);
	}
	fclose(arffHandle);
	return 0;
}