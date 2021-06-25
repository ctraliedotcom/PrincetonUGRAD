#include "R3Shapes/R3Shapes.h"
#include <vector>
using namespace std;

class BlobMeshAligner {
public:
	//Parameters: <mesh filename, point cloud filename, aligned point cloud filename, 
	//				6D or 3D (0/1), maximum # of points to sample>
	BlobMeshAligner(char* i1, char* i2, char* out, bool D3, int m);
	~BlobMeshAligner();

	R3Point* randomSamplePoints();
	int WriteMesh(R3Mesh *mesh, const char *filename);
	R3Mesh* ReadMesh(const char *filename);
	int Align(R3Mesh *mesh1, R3Point* points2, 
				int pca_translation, int pca_rotation, int pca_scale, 
				int icp_translation, int icp_rotation, int icp_scale);
	int ICPAlignmentTransformation(R3Mesh *mesh1, const R3Point *points2, 
									R3Affine& affine12, R3Affine& affine21,
									int translation, int rotation, int scale);
	RNScalar RMSD(R3Mesh *mesh1, R3Point *points2, const R3Affine& affine12, const R3Affine& affine21);
	void CreatePointSurfaceCorrespondences(R3Mesh *mesh1, const R3Point *points2, 
	  const R3Affine& affine12, const R3Affine& affine21,
	  R3Point *correspondences1, R3Point *correspondences2);
	
	//Return the fraction of the mesh within a distance eps of the point cloud
	double getMeshToPointsFrac(double eps, int meshsamples);
	
	//Return the fraction of points that are within a distance eps of the mesh 
	//(**this is the one I actually use)
	double getPointsToMeshFrac(double eps);
	void doAlign(bool save);
	void initMeshTree();
	void translatePointsZ(double blobZTrans, double meshZTrans);
	double getDist();

	vector<R3Point> allpoints;
	char *input1_name;
	char *input2_name;
    char *alignedPoints_name;
	int npoints;
	bool DOF3;//3 degrees of freedom alignment? (XY translation, rotation about z)
	double dist;//"Distance" of aligned point set from mesh

	R3MeshSearchTree* tree1;
	R3Mesh* mesh1;
	R3Point* points2;
	R3Affine affine12;
};
