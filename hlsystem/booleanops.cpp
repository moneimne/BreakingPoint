#include "booleanops.h"

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

void BooleanOps::testBoolean(std::vector<Geometry> voroData) {
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