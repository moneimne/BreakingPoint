#pragma once
#include "voronoi.h"
#include "igl/copyleft/cgal/mesh_boolean.h"

class BooleanOps {
public:
	static Geometry testBoolean(Geometry voroCell, Geometry isectObject, igl::MeshBooleanType boolType);
	static Geometry testBooleanOff(Geometry voroCell, Eigen::MatrixXd VA, Eigen::MatrixXi FA, igl::MeshBooleanType boolType);
};