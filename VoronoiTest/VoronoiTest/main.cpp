// Voronoi calculation example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011
#include <iostream>
#include <fstream>
#include "booleanops.h"
#include <igl/readOFF.h>
using namespace voro;
std::vector<Geometry> makeCube(double xScale, double yScale, double zScale) {
	int i;
	double x, y, z;
	double x_min = -xScale/2.0, x_max = xScale/2.0;
	double y_min = -yScale/2.0, y_max = yScale/2.0;
	double z_min = -zScale/2.0, z_max = zScale/2.0;
	int n_x = 6, n_y = 6, n_z = 6;

	// Create a container with the geometry given above, and make it
	// non-periodic in each of the three coordinates. Allocate space for
	// eight particles within each computational block
	container con(x_min, x_max, y_min, y_max, z_min, z_max, n_x, n_y, n_z,
		false, false, false, 8);

	con.put(0, 0.0, 0.0, 0.0);

	con.print_custom("%w\n%P\n%s\n%t", "cube.txt");
	
	return Voronoi::parseVoronoi("cube.txt");
}

void writeToFile(std::vector<Geometry> mesh, std::string fileName) {
	std::ofstream file;
	file.open(fileName);
	for (int i = 0; i < mesh.size(); i++) {
		Geometry currMesh = mesh.at(i);
		// number of vertices
		file << currMesh.first.size() << std::endl;
		// vertex data
		for (int j = 0; j < currMesh.first.size(); j++) {
			std::vector<double> currVertex = currMesh.first.at(j);
			file << "(" << currVertex[0] << "," << currVertex[1] << "," << currVertex[2] << ")";
			if (j == currMesh.first.size() - 1) {
				file << std::endl;
			}
			else {
				file << " ";
			}
		}
		// number of faces
		file << currMesh.second.size() << std::endl;
		// face data
		for (int j = 0; j < currMesh.second.size(); j++) {
			std::vector<int> currFace = currMesh.second.at(j);
			file << "(";
			for (int k = 0; k < currFace.size(); k++) {
				file << currFace[k];
				if (k < currFace.size() - 1) {
					file << ",";
				}
			}
			file << ")";
			if (j == currMesh.second.size() - 1) {
				file << std::endl;
			}
			else {
				file << " ";
			}
		}
	}
	file.close();
}

int main() {
	std::vector<float> offset = { 0.0f, 0.0f, 0.0f };
	/*Voronoi::testWeb(50, offset, 1.0f, "web.txt");
	std::vector<Geometry> web = Voronoi::parseVoronoi("web.txt");
	std::vector<Geometry> cube = makeCube(1.0, 1.0, 0.02);
	std::vector<Geometry> outputMesh;
	for (int i = 0; i < web.size(); i++) {
		igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_INTERSECT);
		Geometry outputGeometry = BooleanOps::testBoolean(cube.at(0), web.at(i), boolean_type);
		if (outputGeometry.first.size() != 0 && outputGeometry.second.size() != 0) {
			outputMesh.push_back(outputGeometry);
		}
	}
	writeToFile(outputMesh, "webFracture.txt");*/
	Voronoi::createVoronoiFile(20, offset, 1.0f, "voronoi.txt");
	std::vector<Geometry> voroData = Voronoi::parseVoronoi("voronoi.txt");
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	if (!igl::readOFF("C:/Users/moneimne/Desktop/CIS660/BreakingPointProject/BreakingPoint/VoronoiTest/VoronoiTest/nido.off", V, F)) {
		std::cout << "Error reading off\n";
		return -1;
	}
	std::vector<Geometry> outputMesh;
	for (int i = 0; i < voroData.size(); i++) {
		igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_INTERSECT);
		Geometry outputGeometry = BooleanOps::testBooleanOff(voroData.at(i), V, F, boolean_type);
		if (outputGeometry.first.size() != 0 && outputGeometry.second.size() != 0) {
			outputMesh.push_back(outputGeometry);
		}
	}
	writeToFile(outputMesh, "nidoFracture.txt");
}