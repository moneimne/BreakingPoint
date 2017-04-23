#include "voronoi.h"
#include <iostream>
using namespace voro;

double rnd() { return double(rand()) / RAND_MAX; }

void Voronoi::createVoronoiFile(int numSeeds, std::string outputFileName) {
	int i;
	double x, y, z;
	double x_min = -1, x_max = 1;
	double y_min = -1, y_max = 1;
	double z_min = -1, z_max = 1;
	int n_x = 6, n_y = 6, n_z = 6;

	// Create a container with the geometry given above, and make it
	// non-periodic in each of the three coordinates. Allocate space for
	// eight particles within each computational block
	container con(x_min, x_max, y_min, y_max, z_min, z_max, n_x, n_y, n_z,
		false, false, false, 8);

	// Randomly add particles into the container
	for (i = 0; i<numSeeds; i++) {
		x = x_min + rnd()*(x_max - x_min);
		y = y_min + rnd()*(y_max - y_min);
		z = z_min + rnd()*(z_max - z_min);
		con.put(i, x, y, z);
	}

	con.print_custom("%w\n%P\n%s\n%t", outputFileName.c_str());
}

void Voronoi::triangulateFaces(Faces &trifaceData, const Faces &faceData) {
	for (int i = 0; i < faceData.size(); i++) {
		for (int j = 0; j < faceData[i].size() - 2; j++) {
			std::vector<int> tri = { faceData[i][0], faceData[i][j + 2],faceData[i][j + 1] };
			trifaceData.push_back(tri);
		}
	}
}

std::vector<Geometry> Voronoi::parseVoronoi(std::string voronoiFile) {
	std::vector<Geometry> voroData = std::vector<Geometry>();

	FILE *voroFile = fopen(voronoiFile.c_str(), "r");
	if (voroFile == NULL) {
		printf("IOError: %s couldn't be opened.\n", voronoiFile.c_str());
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
				int numFaceCharsRead;
				int f;
				sscanf(face + numFaceChars + 1, "%i%n", &f, &numFaceCharsRead);
				numFaceChars += numFaceCharsRead + 1;
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