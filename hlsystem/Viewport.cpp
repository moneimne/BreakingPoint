#include "Viewport.h"
#include "voro/voro++.hh"

#include "Eigen/Dense"
#include <igl/copyleft/cgal/mesh_boolean.h>

using namespace voro;
using namespace Eigen;

Eigen::MatrixXd VA, VB, VC;
Eigen::VectorXi J, I;
Eigen::MatrixXi FA, FB, FC;
igl::MeshBooleanType boolean_type(
	igl::MESH_BOOLEAN_TYPE_UNION);

const char * MESH_BOOLEAN_TYPE_NAMES[] =
{
	"Union",
	"Intersect",
	"Minus",
	"XOR",
	"Resolve",
};



Viewport::Viewport()
{
}


Viewport::~Viewport()
{
}

void Viewport::handleMouseEvent(UI_Event *event)
{

}

void Viewport::createRay()
{

}

void Viewport::testIntersect(GEO_Detail* collision)
{
	MatrixXd m(2, 2);
	m(0, 0) = 3;
	m(1, 0) = 2.5;
	m(0, 1) = -1;
	GU_RayIntersect* myCollision;
	if (collision)
	{
		voronoicell v;
		UT_Vector3T<float_t> t(0, 0, -2);
			//GEO_PrimList - collection of polygons in one piece of geometry
			// entries() - number of polygons
		GEO_PrimList primlist = collision->primitives();
		int numPolys = primlist.entries();
		std::cout << "Entries: " << numPolys << std::endl;
		for (GA_Offset ptoff : collision->getPointRange()) {
			std::cout << "Point " << collision->pointIndex(ptoff) << " pos " << collision->getPos3(ptoff) << "\n";
		}
		for (GA_Offset primoff : collision->getPrimitiveRange()) {
			std::cout << "Primitive " << collision->primitiveIndex(primoff) << ": ";
			const GEO_Primitive* face = collision->getGEOPrimitive(primoff);
			for (GA_Offset ptoff : face->getPointRange()) {
				std::cout << collision->pointIndex(ptoff) << " ";
			}
			std::cout << "\n";
		}

		/*for (int i = 0; i < numPolys; i++) {
			GEO_Primitive* poly = primlist[i];
			for (GA_Offset ptoff : poly->getPointRange()) {
				
			}
		}*/
		std::cout << "TEST" << std::endl;
		myCollision = new GU_RayIntersect;
		myCollision->init((GU_Detail*)collision);
		UT_Vector3 org(0, 0, 5);
		UT_Vector3 dir(0, 0, -1);
		UT_Vector3 pos;
		GU_Ray ray(org, dir);
		GU_RayInfo info;
		info.reset();
		info.init(10.0, 0.0, GU_FIND_ALL, 1e-4);

		//sendRay loads info with all intersections
		int numHit = myCollision->sendRay(org, dir, info);

		//Another way of getting all polygons by ray intersection
		//gets all polygons regardless of intersection

		UT_ValArray< GU_RayPrimInfo * > primList = myCollision->getPrimList();
		/*for (int i = 0; i < primList.size(); i++) {
			GU_RayPrimInfo * prim_info = primList(i);
			const GEO_Primitive * geo_prim = prim_info->getPrim();
			fpreal area = geo_prim->calcArea();
			std::cout << "area: " << area << std::endl;
		}*/

		GEO_Detail * geo = primList(0)->getPrim()->getParent();
		if (numHit > 0)
		{
			std::cout << "numHit " << numHit << std::endl;
			for (int i = 0; i < numHit; i++) {
				fpreal t;
				std::cout << i << std::endl;
				t = (*info.myHitList)(i).t;
				UT_Vector3 isect = org + dir*t;
				std::cout << "x: " << isect[0] << " y: " << isect[1] << " z: " << isect[2] << std::endl;
			}
		}
	}

}