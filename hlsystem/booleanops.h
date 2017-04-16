#pragma once
#include "voronoi.h"
#include "igl/copyleft/cgal/mesh_boolean.h"

class BooleanOps {
public:
	void testBoolean(std::vector<Geometry> voroData);
};