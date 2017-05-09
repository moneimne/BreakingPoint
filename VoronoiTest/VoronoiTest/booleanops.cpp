#include "booleanops.h"
#include "igl/list_to_matrix.h"
#include "igl/matrix_to_list.h"

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

Geometry BooleanOps::testBoolean(Geometry isectObject, Geometry voroCell, igl::MeshBooleanType boolType) {
	Eigen::MatrixXd VA, VB, VC;
	Eigen::MatrixXi FA, FB, FC;
	igl::list_to_matrix(isectObject.first, VA);
	igl::list_to_matrix(isectObject.second, FA);
	igl::list_to_matrix(voroCell.first, VB);
	igl::list_to_matrix(voroCell.second, FB);
	//std::cout << "VA: " << VA << std::endl;
	//std::cout << "FA: " << FA << std::endl;
	//std::cout << "VB: " << VB << std::endl;
	//std::cout << "FB: " << FB << std::endl;
	//std::cout << "converted everything to matrices\n";
	if (!igl::copyleft::cgal::mesh_boolean(VA, FA, VB, FB, boolType, VC, FC)) {
		//std::cout << "failed boolean\n";
		return Geometry();
	}
	//std::cout << "VC: " << VC << std::endl;
	//std::cout << "FC: " << FC << std::endl;
	//std::cout << "succeeded boolean\n";
	Points outputPoints;
	Faces outputFaces;
	igl::matrix_to_list(VC, outputPoints);
	igl::matrix_to_list(FC, outputFaces);
	//std::cout << outputPoints.size() << ", " << outputFaces.size() << std::endl;
	return Geometry(outputPoints, outputFaces);
}

Geometry BooleanOps::testBooleanOff(Geometry voroCell, Eigen::MatrixXd VA, Eigen::MatrixXi FA, igl::MeshBooleanType boolType) {
	Eigen::MatrixXd VB, VC;
	Eigen::MatrixXi FB, FC;
	igl::list_to_matrix(voroCell.first, VB);
	igl::list_to_matrix(voroCell.second, FB);
	if (!igl::copyleft::cgal::mesh_boolean(VA, FA, VB, FB, boolType, VC, FC)) {
		return Geometry();
	}
	Points outputPoints;
	Faces outputFaces;
	igl::matrix_to_list(VC, outputPoints);
	igl::matrix_to_list(FC, outputFaces);
	return Geometry(outputPoints, outputFaces);
}