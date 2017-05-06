// Voronoi calculation example code
//
// Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

#include "booleanops.h"
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

int main() {
	std::vector<float> offset = { 0.0f, 0.0f, 0.0f };
	Voronoi::testWeb(50, offset, 1.0f, "web.txt");
	std::vector<Geometry> web = Voronoi::parseVoronoi("web.txt");
	std::vector<Geometry> cube = makeCube(1.0, 1.0, 0.02);

}