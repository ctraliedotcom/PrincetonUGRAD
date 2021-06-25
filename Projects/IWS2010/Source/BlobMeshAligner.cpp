#include "BlobMeshAligner.h"

// Program arguments


static int pca_translation = 1;
static int pca_scale = 0;
static int pca_rotation = 1; // 2 = only 180s
static int icp_translation = 1;
static int icp_scale = 0;
static int icp_rotation = 1;
static int print_verbose = 0;
static int print_debug = 0;
static int max_points;

void printAffine(R3Affine affine) {
	const R4Matrix& m = affine.Matrix();
	printf("  Matrix[0][0-3] = %g %g %g %g\n", m[0][0], m[0][1], m[0][2], m[0][3]);
	printf("  Matrix[1][0-3] = %g %g %g %g\n", m[1][0], m[1][1], m[1][2], m[1][3]);
	printf("  Matrix[2][0-3] = %g %g %g %g\n", m[2][0], m[2][1], m[2][2], m[2][3]);
	printf("  Matrix[3][0-3] = %g %g %g %g\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

void BlobMeshAligner::CreatePointSurfaceCorrespondences(
  R3Mesh *mesh1, const R3Point *points2, 
  const R3Affine& affine12, const R3Affine& affine21,
  R3Point *correspondences1, R3Point *correspondences2)
{
  // Compute correspondences for points2 -> mesh1
  //Only create mesh search tree once since it's a costly operation
  if (!tree1) tree1 = new R3MeshSearchTree(mesh1);
  else assert(mesh1 == tree1->mesh);
  for (int i = 0; i < npoints; i++) {
    R3Point position2 = points2[i];
    position2.Transform(affine21);
    R3MeshIntersection closest;
    tree1->FindClosest(position2, closest);
    correspondences1[i] = closest.point;
    correspondences2[i] = points2[i];
	if (DOF3) {
		//ADDED BY CHRIS TRALIE:
		//If 3DOF alignment is being done, set the z coordinates to zero
		correspondences1[i].SetCoord(2, 0);
		correspondences2[i].SetCoord(2, 0);
	}
  }
}


RNScalar BlobMeshAligner::RMSD(R3Mesh *mesh1, R3Point *points2, const R3Affine& affine12, const R3Affine& affine21)
{
  // Create array of correspondences
  R3Point *correspondences1 = new R3Point [npoints];
  R3Point *correspondences2 = new R3Point [npoints];
  CreatePointSurfaceCorrespondences(mesh1, points2, affine12, affine21,
									correspondences1, correspondences2);

  // Add SSD of correspondences
  RNScalar ssd = 0;
  for (int i = 0; i < npoints; i++) {
    R3Point position1 = correspondences1[i];
    position1.Transform(affine12);
    R3Point& position2 = correspondences2[i];
    RNScalar d = R3Distance(position1, position2);
    ssd += d * d;
  }

  // Compute the RMSD
  RNScalar rmsd = sqrt(ssd / npoints);

  // Delete arrays of correspondences
  delete [] correspondences1;
  delete [] correspondences2;

  // Return RMSD
  return rmsd;
}



int BlobMeshAligner::ICPAlignmentTransformation(
  R3Mesh *mesh1, const R3Point *points2, R3Affine& affine12, R3Affine& affine21,
  int translation, int rotation, int scale)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate arrays of correspondences
  int ncorrespondences = npoints;
  R3Point *correspondences_buffer1 = new R3Point [2 * ncorrespondences];
  R3Point *correspondences_buffer2 = new R3Point [2 * ncorrespondences];
  assert(correspondences_buffer1 && correspondences_buffer2);

  // Iterate until max_iterations or converged
  RNBoolean converged = FALSE;
  const int max_iterations = 128;
  for (int iteration = 0; iteration < max_iterations; iteration++) {
    // Get arrays of correspondences
    R3Point *correspondences1 = &correspondences_buffer1[iteration%2 * ncorrespondences];
    R3Point *correspondences2 = &correspondences_buffer2[iteration%2 * ncorrespondences];

    // Update correspondences for aligning transformation
    CreatePointSurfaceCorrespondences(
      mesh1, points2, 
      affine12, affine21, 
      correspondences1, correspondences2);

    // Update aligning transformation for correspondences
    R4Matrix matrix = R3AlignPoints(ncorrespondences, correspondences2, correspondences1, 
      NULL, translation, rotation, scale);
    affine12.Reset(matrix);
    affine21 = affine12.Inverse();

    // Check for convergence
    converged = FALSE;
    if (iteration > 0) {
      converged = TRUE;
      R3Point *prev_correspondences1 = &correspondences_buffer1[(1-(iteration%2)) * ncorrespondences];
      R3Point *prev_correspondences2 = &correspondences_buffer2[(1-(iteration%2)) * ncorrespondences];
      for (int i = 0; i < ncorrespondences; i++) {
        if (!R3Contains(correspondences1[i], prev_correspondences1[i])) { converged = FALSE; break; }
        if (!R3Contains(correspondences2[i], prev_correspondences2[i])) { converged = FALSE; break; }
      }
    }
    if (converged) break;
  }

  delete[] correspondences_buffer1;
  delete[] correspondences_buffer2;

  // Return whether converged
  return converged;
}



int BlobMeshAligner::Align(R3Mesh *mesh1, R3Point* points2, 
  int pca_translation, int pca_rotation, int pca_scale, 
  int icp_translation, int icp_rotation, int icp_scale)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Initialize transformation
  affine12 = R3Affine(R3identity_affine);
  R3Affine affine21(R3identity_affine);
  RNBoolean converged = TRUE;
  RNScalar rmsd = FLT_MAX;

  // Compute info for mesh1
  R3Point centroid1 = mesh1->Centroid();
  RNScalar radius1 = (pca_scale) ? mesh1->AverageRadius(&centroid1) : 1;
  RNScalar scale1 = (pca_scale && (radius1 > 0)) ? 1.0 / radius1 : 1;
  R3Triad axes1 = (pca_rotation==1) ? mesh1->PrincipleAxes(&centroid1) : R3xyz_triad;
  if (scale1 == 0) {
    fprintf(stderr, "Unable to process first mesh\n");
    return 0;
  }

  // Compute info for points2
  R3Point centroid2 = R3Centroid(npoints, points2);
  RNScalar scale2 = (pca_scale) ? R3AverageDistance(centroid2, npoints, points2) : 1;
  R3Triad axes2 = (pca_rotation == 1) ? R3PrincipleAxes(centroid2, npoints, points2) : R3xyz_triad;
  R3Affine affine02 = R3identity_affine;
  //Added by Chris Tralie: Only do the translation by the centroid in 6DOF alignment.
  //Otherwise, the blob is tranlsated to ground position by translatePointsZ()
  if (pca_translation) affine02.Translate(centroid2.Vector());
  if (pca_rotation==1) affine02.Transform(axes2.Matrix());
  if (pca_scale) affine02.Scale(scale2);
  if (scale2 == 0) {
    fprintf(stderr, "Unable to process first mesh\n");
    return 0;
  }

  // Compute RMSD for alignment with all flips of principle axes
  if (pca_rotation) {
    for (int dim1 = 0; dim1 < 3; dim1++) {
      for (int dim2 = 0; dim2 < 3; dim2++) {
        if (dim1 == dim2) continue;
        for (int dir1 = 0; dir1 < 2; dir1++) {
          for (int dir2 = 0; dir2 < 2; dir2++) {
            // Create triad of axes for flip
            R3Vector axis1a = (dir1 == 1) ? axes1[dim1] : -axes1[dim1];
            R3Vector axis1b = (dir2 == 1) ? axes1[dim2] : -axes1[dim2];
            R3Vector axis1c = axis1a % axis1b;
            R3Triad triad1(axis1a, axis1b, axis1c);

            // Compute transformation for mesh1 (with flip)
            R3Affine affine10 = R3identity_affine;
            if (pca_scale) affine10.Scale(scale1);
            affine10.Transform(triad1.InverseMatrix());
			if (pca_translation) affine10.Translate(-centroid1.Vector());

            // Compute composite transformation and its inverse
            R3Affine flipped_affine12 = affine02;
            flipped_affine12.Transform(affine10);
            R3Affine flipped_affine21 = flipped_affine12.Inverse();

            // Refine alignment with ICP
            RNBoolean flipped_converged = FALSE;
            if (icp_translation || icp_rotation || icp_scale) {
              flipped_converged = ICPAlignmentTransformation(
                mesh1, points2, 
                flipped_affine12, flipped_affine21, 
                icp_translation, icp_rotation, icp_scale);
            }

            // Compute RMSD for transformation
            RNScalar flipped_rmsd = RMSD(
              mesh1, points2, 
              flipped_affine12, flipped_affine21);

            // Check if best so far -- if so, save
            if (flipped_rmsd < rmsd) {
              affine12 = flipped_affine12;
              affine21 = flipped_affine21;
              converged = flipped_converged;
              rmsd = flipped_rmsd;
            }

            // Print statistics
            if (print_debug) {
              printf("Computed alignment transformation for flip %d %d %d %d ...\n", dim1, dim2, dir1, dir2);
              printf("  Time = %.2f seconds\n", start_time.Elapsed());
              printf("  Max Points = %d\n", max_points);
			  printAffine(flipped_affine12);
              printf("  Scale = %g\n", flipped_affine12.ScaleFactor());
              printf("  Converged = %d\n", flipped_converged);
              printf("  RMSD = %g\n", flipped_rmsd);
              fflush(stdout);
            }
          }
        }
      }
    }
  }
  else {
    // Compute transformation for mesh1
    R3Affine affine10 = R3identity_affine;
    if (pca_scale) affine10.Scale(scale1);
    if (pca_translation) affine10.Translate(-centroid1.Vector());

    // Compute composite transformation
    affine12 = affine02;
    affine12.Transform(affine10);
    affine21 = affine12.Inverse();

    // Refine alignment with ICP
    if (icp_translation || icp_rotation || icp_scale) {
      converged = ICPAlignmentTransformation(
        mesh1, points2,
        affine12, affine21,
        icp_translation, icp_rotation, icp_scale);
    }

    // Compute RMSD
    rmsd = RMSD(
      mesh1, points2, 
      affine12, affine21);
  }

  if (DOF3) {
	  //Added by Chris Tralie: Make sure that for 3DOF alignment, 
	  //the matrix only has zy translation and rotation about z-axis
	  R4Matrix m = affine21.Matrix();
	  m[0][2] = 0;
	  m[1][2] = 0;
	  m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
	  m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	  affine21 = R3Affine(m);
	  //printf("\n\n");
	  //printAffine(affine21);
	  //printf("\n\n");
  }

  //**** This is where I take the points to the mesh **///
  // Apply transformation to take the points to the mesh
  for (int i = 0; i < npoints; i++) {
		points2[i].Transform(affine21);
  }

  for (size_t i = 0; i < allpoints.size(); i++) {
		allpoints[i].Transform(affine21);
  }

  // Print statistics
  if (print_verbose) {
    printf("Computed alignment transformation ...\n");
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
	printAffine(affine12);
	printf("  Scale = %g\n", affine12.ScaleFactor());
    printf("  Converged = %d\n", converged);
    printf("  RMSD = %g\n", rmsd);
    fflush(stdout);
  }

  // Return success
  return 1;
}



R3Mesh* BlobMeshAligner::ReadMesh(const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate mesh
  R3Mesh *mesh = new R3Mesh();
  assert(mesh);

  // Read mesh from file
  if (!mesh->ReadFile(filename)) {
    delete mesh;
    return NULL;
  }

  // Check if mesh is valid
  assert(mesh->IsValid());

  // Print statistics
  if (print_verbose) {
    printf("Read mesh from %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Faces = %d\n", mesh->NFaces());
    printf("  # Edges = %d\n", mesh->NEdges());
    printf("  # Vertices = %d\n", mesh->NVertices());
    fflush(stdout);
  }

  // Return mesh
  return mesh;
}


int BlobMeshAligner::WriteMesh(R3Mesh *mesh, const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Write mesh to file
  if (!mesh->WriteFile(filename)) {
    return 0;
  }

  // Print statistics
  if (print_verbose) {
    printf("Wrote mesh to %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Faces = %d\n", mesh->NFaces());
    printf("  # Edges = %d\n", mesh->NEdges());
    printf("  # Vertices = %d\n", mesh->NVertices());
    fflush(stdout);
  }

  // Return succes
  return 1;
}

//Load points from "input2_name" and randomly sample
R3Point* BlobMeshAligner::randomSamplePoints() {
	RNSeedRandomScalar();
	vector<R3Point> vpoints;
	FILE* f = fopen(input2_name, "r");
	if (f == NULL) {
		fprintf(stderr, "ERROR Opening file %s\n", input2_name);
		return NULL;
	}
	while (!feof(f)) {
		double x, y, z;
		fscanf(f, "%lf%lf%lf", &x, &y, &z);
		R3Point P(x, y, z);
		vpoints.push_back(P);
		allpoints.push_back(P);
	}
	//Figure out how many points to use
	npoints = min(max_points, (int)vpoints.size());
	R3Point* ret = new R3Point[npoints];
	//Randomly sample points and return
	for (int i = 0; i < npoints; i++) {
		size_t index = (size_t)(RNRandomScalar() * vpoints.size());
		if (index < 0) index = 0;
		if (index >= vpoints.size()) index = vpoints.size() - 1;
		ret[i] = vpoints[index];
		vpoints[index] = vpoints[vpoints.size() - 1];
		vpoints.pop_back();
	}
	fclose(f);
	return ret;
}



double BlobMeshAligner::getMeshToPointsFrac(double eps, int meshsamples) {
	//First randomly sample some points from the mesh

	// Sample faces
	// Count total weighted area of faces
	RNArea total_area = 0.0;
	for (int i = 0; i < mesh1->NFaces(); i++) {
	  R3MeshFace *face = mesh1->Face(i);
	  RNScalar face_area = mesh1->FaceArea(face);
	  mesh1->SetFaceValue(face, face_area);
	  total_area += face_area;
	}
	R3Point* meshpoints = new R3Point[meshsamples];

	// Generate points with a uniform distribution over surface area
	int index = 0;
	RNSeedRandomScalar();
	for (int i = 0; i < mesh1->NFaces(); i++){
	  R3MeshFace *face = mesh1->Face(i);

	  // Get vertex positions
	  R3MeshVertex *v0 = mesh1->VertexOnFace(face, 0);
	  R3MeshVertex *v1 = mesh1->VertexOnFace(face, 1);
	  R3MeshVertex *v2 = mesh1->VertexOnFace(face, 2);
	  const R3Point& p0 = mesh1->VertexPosition(v0);
	  const R3Point& p1 = mesh1->VertexPosition(v1);
	  const R3Point& p2 = mesh1->VertexPosition(v2);

	  // Determine number of points for face 
	  RNScalar ideal_face_npoints = meshsamples * mesh1->FaceValue(face) / total_area;
	  int face_npoints = (int) ideal_face_npoints;
	  RNScalar remainder = ideal_face_npoints - face_npoints;
	  if (remainder > RNRandomScalar()) face_npoints++;

	  // Generate random points in face
	  for (int j = 0; j < face_npoints; j++) {
		RNScalar r1 = sqrt(RNRandomScalar());
		RNScalar r2 = RNRandomScalar();
		RNScalar t0 = (1.0 - r1);
		RNScalar t1 = r1 * (1.0 - r2);
		RNScalar t2 = r1 * r2;
		meshpoints[index++] = t0*p0 + t1*p1 + t2*p2;
		if (index >= meshsamples) break;
	  }

	  // Check number of points created
	  if (index >= meshsamples) break;
	}

	//Now find the closest point in the point cloud to each sampled mesh point
	R3Kdtree<const R3Point*>* tree1;
	RNArray<const R3Point *> array1;
	for (int i = 0; i < meshsamples; i++) array1.Insert(&meshpoints[i]);
		tree1 = new R3Kdtree<const R3Point *>(array1);

	// Build kdtree for points2
	static R3Kdtree<const R3Point *>* tree2;
	RNArray<const R3Point *> array2;
	for (int i = 0; i < npoints; i++) array2.Insert(&points2[i]);
		tree2 = new R3Kdtree<const R3Point *>(array2);

	int closenum = 0;//The number of points within eps
	for (int i = 0; i < meshsamples; i++) {
		R3Point position1 = meshpoints[i];
		const R3Point *closest = tree2->FindClosest(position1);
		if (closest == NULL)
			continue;
		R3Vector diff = (*closest) - position1;
		double dist = diff.Length();
		/*printf("(%.3lf, %.3lf, %.3lf), (%.3lf, %.3lf, %.3lf), %.3lf \n", 
			closest->X(), closest->Y(), closest->Z(),
			position1.X(), position1.Y(), position1.Z(), dist);*/
		if (dist < eps)
			closenum++;
	}

	delete[] meshpoints;
	delete tree1;
	delete tree2;

	return (double)closenum / (double)meshsamples;
}

/*R3Point myFindClosest(R3Mesh* mesh, R3Point p) {
	double smallestDist;
	int smallestIndex = 0;
	R3Vector diff = (mesh->Vertex(0))->position - p;
	smallestDist = diff.Length();
	for (int i = 1; i < mesh->NVertices(); i++) {
		diff = (mesh->Vertex(i))->position - p;
		if (diff.Length() < smallestDist) {
			smallestIndex = i;
			smallestDist = diff.Length();
		}
	}
	return (mesh->Vertex(smallestIndex))->position;
}*/

//Determine the fraction of points that are within a distance eps of the mesh
double BlobMeshAligner::getPointsToMeshFrac(double eps) {
	//FILE* f = fopen("closest.xyz", "w");
	if (tree1 == NULL)
		tree1 = new R3MeshSearchTree(mesh1);

	int numclose = 0;
	dist = 0;
	for (size_t i = 0; i < allpoints.size(); i++) {
		R3Point position2 = allpoints[i];
		R3MeshIntersection closest;
		//The points have been transformed to align with the mesh, so
		//we should be able to use the original mesh search tree
		tree1->FindClosest(position2, closest);
		R3Vector diff = closest.point - position2;
		dist += diff.Length();

		if (diff.Length() < eps) {
			numclose++;
		}
	}
	//fclose(f);
	return (double)numclose / (double)allpoints.size();
}


double BlobMeshAligner::getDist() {
	return dist;
}

void BlobMeshAligner::doAlign(bool save) {
    // Align mesh1 to points2 point set
	if (points2 == NULL) {
		fprintf(stderr, "ERROR: Point set null while trying to align!\n\n");
		return;
	}
    if (!Align(mesh1, points2,
      pca_translation, pca_rotation, pca_scale, 
	  icp_translation, icp_rotation, icp_scale)) {
		  fprintf(stderr, "ERROR: Unable to align points");
		  return;
	}
	if (alignedPoints_name != NULL && save) {
		//If the user has chosen to save the result of alignment to a file
		//Output the aligned point set
		FILE* f = fopen(alignedPoints_name, "w");
		if (!f) {
			fprintf(stderr, "ERROR: Unable to open file %s for outputting aligned point set\n", alignedPoints_name);
			return;
		}
		for (int i = 0; i < npoints; i++) {
			fprintf(f, "%lf %lf %lf\n", points2[i].X(), points2[i].Y(), points2[i].Z());
		}
		fclose(f);
	}
}


void BlobMeshAligner::initMeshTree() {
	tree1 = new R3MeshSearchTree(mesh1);
}

void BlobMeshAligner::translatePointsZ(double blobZTrans, double meshZTrans) {
	//printf("\n\nZTranslation = %.3f\n\n", meshZTrans - blobZTrans);
	R3Vector translate(0, 0, meshZTrans - blobZTrans);
	//Translate object back to 0 position on ground for comparison
	for (int i = 0; i < npoints; i++) {
		points2[i].Translate(translate);
	}
}

//Pass NULL as o1 if the program should not output
BlobMeshAligner::BlobMeshAligner(char* i1, char* i2, char* out, bool D3, int m)
{
  input1_name = i1;
  input2_name = i2;
  alignedPoints_name = out;
  tree1 = NULL;
  mesh1 = NULL;
  points2 = NULL;
  DOF3 = D3;
  max_points = m;
  affine12 = R3Affine(R3identity_affine);

  // Read mesh1
  mesh1 = ReadMesh(input1_name);
  if (!mesh1) {
		fprintf(stderr, "ERROR reading mesh %s\n", input1_name);
		return;
  }

  // Check if aligning to second model
  if (input2_name) {
    points2 = randomSamplePoints();
	if (!points2) {
		fprintf(stderr, "ERROR: Unable to randomly sample points\n");
		return;
	}
  }
}

BlobMeshAligner::~BlobMeshAligner() {
	if (tree1 != NULL)
		delete tree1;
	if (mesh1 != NULL)
		delete mesh1;
	if (points2 != NULL)
		delete points2;
}
