// Voronoi calculation example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

#include "voro/voro++.hh"
using namespace voro;

// Set up constants for the container geometry
const double x_min = -1, x_max = 1;
const double y_min = -1, y_max = 1;
const double z_min = -1, z_max = 1;
const double cvol = (x_max - x_min)*(y_max - y_min)*(x_max - x_min);

// Set up the number of blocks that the container is divided into
const int n_x = 6, n_y = 6, n_z = 6;

// Set the number of particles that are going to be randomly introduced
const int particles = 3;

// This function returns a random double between 0 and 1
double rnd() { return double(rand()) / RAND_MAX; }

void testBoolean() {

}

std::vector<std::pair<std::vector<std::vector<float>>, std::vector<std::vector<int>>>> testParse(std::string inputFile) {
	std::vector<std::pair<std::vector<std::vector<float>>, std::vector<std::vector<int>>>> voroData = std::vector<std::pair<std::vector<std::vector<float>>, std::vector<std::vector<int>>>>();
	
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
		std::vector<std::vector<float>> vertexData = std::vector<std::vector<float>>();
		fgets(line, 1000, voroFile);
		int numChars = 0;
		for (int i = 0; i < numVertices; i++) {
			float x, y, z;
			int numCharsRead;
			sscanf(line + numChars, "(%f,%f,%f) %n", &x, &y, &z, &numCharsRead);
			numChars += numCharsRead;

			std::vector<float> vertex = std::vector<float>();
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
		std::vector<std::vector<int>> faceData = std::vector<std::vector<int>>();
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

		std::pair<std::vector<std::vector<float>>, std::vector<std::vector<int>>> pairData = std::pair<std::vector<std::vector<float>>, std::vector<std::vector<int>>>(vertexData, faceData);
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
	testVoro();
	std::vector<std::pair<std::vector<std::vector<float>>, std::vector<std::vector<int>>>> voroData = testParse("output.txt");
	printf("done!\n");
}