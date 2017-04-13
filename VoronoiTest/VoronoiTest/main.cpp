// Voronoi calculation example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

#include "voro/voro++.hh"
#include "igl/copyleft/cgal/mesh_boolean.h"
#include "igl/list_to_matrix.h"
using namespace voro;

typedef std::vector<std::vector<int>> Faces;
typedef std::vector<std::vector<double>> Points;
typedef std::pair<Points, Faces> Geometry;

// Set up constants for the container geometry
const double x_min = -1, x_max = 1;
const double y_min = -1, y_max = 1;
const double z_min = -1, z_max = 1;
const double cvol = (x_max - x_min)*(y_max - y_min)*(x_max - x_min);

// Set up the number of blocks that the container is divided into
const int n_x = 6, n_y = 6, n_z = 6;

// Set the number of particles that are going to be randomly introduced
const int particles = 2;

// This function returns a random double between 0 and 1
double rnd() { return double(rand()) / RAND_MAX; }

void buildCube(double offset, const char* filename) {
	container con(x_min + offset, x_max + offset, y_min + offset, y_max + offset, z_min + offset, z_max + offset, n_x, n_y, n_z,
		false, false, false, 8);

	con.put(0, 0.0, 0.0, 0.0);

	con.print_custom("%w\n%P\n%s\n%t", filename);
}

void vertexListToMatrix(const Points& vertexData, Eigen::MatrixXd& M) {
	M.resize(vertexData.size(), 3);
	for (int i = 0; i < vertexData.size(); i++) {
		M(i, 0) = vertexData[i][0];
		M(i, 1) = vertexData[i][1];
		M(i, 2) = vertexData[i][2];
	}
}

void faceListToMatrix(const Faces& faceData, Eigen::MatrixXi& M) {
	M.resize(faceData.size(), 3);
	for (int i = 0; i < faceData.size(); i++) {
		M(i, 0) = faceData[i][0];
		M(i, 1) = faceData[i][1];
		M(i, 2) = faceData[i][2];
	}
}

void testBoolean(std::vector<Geometry> voroData) {
	Eigen::MatrixXd VA, VB, VC;
	Eigen::MatrixXi FA, FB, FC;
	igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_INTERSECT);
	vertexListToMatrix(voroData[0].first, VA);
	faceListToMatrix(voroData[0].second, FA);
	vertexListToMatrix(voroData[1].first, VB);
	faceListToMatrix(voroData[1].second, FB);
	std::cout << "VA:\n";
	std::cout << VA << std::endl;
	std::cout << "FA:\n";
	std::cout << FA << std::endl;
	std::cout << "VB:\n";
	std::cout << VB << std::endl;
	std::cout << "FB:\n";
	std::cout << FB << std::endl;
	if (igl::copyleft::cgal::mesh_boolean(VA, FA, VB, FB, boolean_type, VC, FC)) {
		std::cout << "Success\n";
	}
	std::cout << "VC:\n";
	std::cout << VC << std::endl;
	std::cout << "FC:\n";
	std::cout << FC << std::endl;
}


void triangulateFaces(Faces &trifaceData, const Faces &faceData) {
	for (int i = 0; i < faceData.size(); i++) {
		for (int j = 0; j < faceData[i].size() - 2; j++) {
			std::vector<int> tri = { faceData[i][0], faceData[i][j + 2],faceData[i][j + 1]};
			trifaceData.push_back(tri);
		}
	}
}

std::vector<Geometry> testParse(std::string inputFile) {
	std::vector<Geometry> voroData = std::vector<Geometry>();
	
	FILE *voroFile = fopen(inputFile.c_str(), "r");
	if (voroFile == NULL) {
		printf("IOError: %s couldn't be opened.\n", inputFile.c_str());
		return voroData;
	}

	char line[1000];
	
	fgets(line, 1000, voroFile);
	while (true) {
		// Get number of vertices
		int numVertices;
		sscanf(line, "%i", &numVertices);

		// Get vertices
		Points vertexData = Points();
		fgets(line, 1000, voroFile);
		int numChars = 0;
		for (int i = 0; i < numVertices; i++) {
			double x, y, z;
			int numCharsRead;
			sscanf(line + numChars, "(%lf,%lf,%lf) %n", &x, &y, &z, &numCharsRead);
			numChars += numCharsRead;

			std::vector<double> vertex = std::vector<double>();
			vertex.push_back(x);
			vertex.push_back(y);
			vertex.push_back(z);

			vertexData.push_back(vertex);
		}

		// Get number of faces
		int numFaces;
		fgets(line, 1000, voroFile);
		sscanf(line, "%i", &numFaces);

		// Get face indices
		Faces faceData = Faces();
		fgets(line, 1000, voroFile);
		numChars = 0;
		for (int i = 0; i < numFaces; i++) {
			// Get face data
			char face[1000];
			int numCharsRead;
			sscanf(line + numChars, "%s %n", &face, &numCharsRead);
			numChars += numCharsRead;

			// Save indices
			int numIndices = (numCharsRead / 2) - 1;
			int numFaceChars = 0;
			std::vector<int> vertexIndices = std::vector<int>();
			for (int j = 0; j < numIndices; j++) {
				int f;
				sscanf(face + numFaceChars + 1, "%i", &f);
				numFaceChars += 2;
				vertexIndices.push_back(f);
			}

			faceData.push_back(vertexIndices);
		}
		Faces trifaceData;
		triangulateFaces(trifaceData, faceData);
		Geometry pairData = Geometry(vertexData, trifaceData);
		voroData.push_back(pairData);

		// Termination
		if (!fgets(line, 1000, voroFile)) {
			break;
		}
	}
	
	return voroData;
}

void testVoro() {
	int i;
	double x, y, z;

	// Create a container with the geometry given above, and make it
	// non-periodic in each of the three coordinates. Allocate space for
	// eight particles within each computational block
	container con(x_min, x_max, y_min, y_max, z_min, z_max, n_x, n_y, n_z,
		false, false, false, 8);

	// Randomly add particles into the container
	for (i = 0; i<particles; i++) {
		x = x_min + rnd()*(x_max - x_min);
		y = y_min + rnd()*(y_max - y_min);
		z = z_min + rnd()*(z_max - z_min);
		con.put(i, x, y, z);
	}

	con.print_custom("%w\n%P\n%s\n%t", "output.txt");

	// Sum up the volumes, and check that this matches the container volume
	double vvol = con.sum_cell_volumes();
	printf("Container volume : %g\n"
		"Voronoi volume   : %g\n"
		"Difference       : %g\n", cvol, vvol, vvol - cvol);

	// Output the particle positions in gnuplot format
	con.draw_particles("random_points_p.gnu");

	// Output the Voronoi cells in gnuplot format
	con.draw_cells_gnuplot("random_points_v.gnu");
}



int main() {
	buildCube(0.0, "cube1.txt");
	std::vector<Geometry> voroData1 = testParse("cube1.txt");
	buildCube(0.3, "cube2.txt");
	std::vector<Geometry> voroData2 = testParse("cube2.txt");
	std::vector<Geometry> voroData;
	voroData.push_back(voroData1[0]);
	voroData.push_back(voroData2[0]);
	testBoolean(voroData);
}