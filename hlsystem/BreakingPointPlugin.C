


#include <UT/UT_DSOVersion.h>
//#include <RE/RE_EGLServer.h>


#include <UT/UT_Math.h>
#include <UT/UT_Interrupt.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <PRM/PRM_Range.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <PRM/PRM_ChoiceList.h>

#include <limits.h>
#include "BreakingPointPlugin.h"
#include "Viewport.h"
#include "booleanops.h"
using namespace HDK_Sample;

//
// Help is stored in a "wiki" style text file. 
//
// See the sample_install.sh file for an example.
//
// NOTE : Follow this tutorial if you have any problems setting up your visual studio 2008 for Houdini 
//  http://www.apileofgrains.nl/setting-up-the-hdk-for-houdini-12-with-visual-studio-2008/


///
/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case we add ourselves
/// to the specified operator table.
///
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(
	    new OP_Operator("CustomBP",					// Internal name
			    "BreakingPoint",					// UI name
			     SOP_BreakingPoint::myConstructor,	// How to build the SOP
			     SOP_BreakingPoint::myTemplateList,	// My parameters
			     1,				// Min # of sources
			     1,				// Max # of sources
			     SOP_BreakingPoint::myVariables,	// Local variables
			     OP_FLAG_GENERATOR)		// Flag it as generator
	    );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//PUT YOUR CODE HERE
//You need to declare your parameters here
//Example to declare a variable for angle you can do like this :
//static PRM_Name		angleName("angle", "Angle");
static PRM_Name piecesName("pieces", "Number of Pieces");
static PRM_Name scaleName("scale", "Voronoi Scale");
static PRM_Name oriName("origin", "Fracture Origin");
static PRM_Name dirName("direction", "Fracture Direction");


static PRM_Name         sopOrdinalName("choice", "Fracture Type");
static PRM_Name         sopOrdChoices[] =
{
	PRM_Name("choice1", "Spider Web"),
	PRM_Name("choice2", "Dynamic Fracture"),
	PRM_Name(0)
};
static PRM_ChoiceList   sopOrdinalMenu(PRM_CHOICELIST_SINGLE, sopOrdChoices);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				     ^^^^^^^^    ^^^^^^^^^^^^^^^
//				     internal    descriptive version


// PUT YOUR CODE HERE
// You need to setup the initial/default values for your parameters here
// For example : If you are declaring the inital value for the angle parameter
// static PRM_Default angleDefault(30.0);	

static PRM_Default piecesDefault(4);
static PRM_Range piecesRange(PRM_RANGE_UI, 4, PRM_RANGE_UI, 20);
static PRM_Default scaleDefault(1.0);
static PRM_Range scaleRange(PRM_RANGE_UI, 1.0, PRM_RANGE_UI, 50.0);
static PRM_Default oriDefault(0.0);


////////////////////////////////////////////////////////////////////////////////////////

PRM_Template
SOP_BreakingPoint::myTemplateList[] = {
	// PUT YOUR CODE HERE
	// You now need to fill this template with your parameter name and their default value
	// EXAMPLE : For the angle parameter this is how you should add into the template
	// PRM_Template(PRM_FLT,	PRM_Template::PRM_EXPORT_MIN, 1, &angleName, &angleDefault, 0),
	// Similarly add all the other parameters in the template format here
		PRM_Template(PRM_INT, PRM_Template::PRM_EXPORT_MIN, 1, &piecesName, &piecesDefault, 0, &piecesRange),
		PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &scaleName, &scaleDefault, 0, &scaleRange),
		PRM_Template(PRM_ORD,    1, &sopOrdinalName, 0, &sopOrdinalMenu),
		PRM_Template(PRM_XYZ, 3, &oriName),
		PRM_Template(PRM_XYZ, 3, &dirName),
		/////////////////////////////////////////////////////////////////////////////////////////////

    PRM_Template()
};


// Here's how we define local variables for the SOP.
enum {
	VAR_PT,		// Point number of the star
	VAR_NPT		// Number of points in the star
};

CH_LocalVariable
SOP_BreakingPoint::myVariables[] = {
    { "PT",	VAR_PT, 0 },		// The table provides a mapping
    { "NPT",	VAR_NPT, 0 },		// from text string to integer token
    { 0, 0, 0 },
};

bool
SOP_BreakingPoint::evalVariableValue(fpreal &val, int index, int thread)
{
    // myCurrPoint will be negative when we're not cooking so only try to
    // handle the local variables when we have a valid myCurrPoint index.
    if (myCurrPoint >= 0)
    {
	// Note that "gdp" may be null here, so we do the safe thing
	// and cache values we are interested in.
	switch (index)
	{
	    case VAR_PT:
		val = (fpreal) myCurrPoint;
		return true;
	    case VAR_NPT:
		val = (fpreal) myTotalPoints;
		return true;
	    default:
		/* do nothing */;
	}
    }
    // Not one of our variables, must delegate to the base class.
    return SOP_Node::evalVariableValue(val, index, thread);
}

OP_Node *
SOP_BreakingPoint::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_BreakingPoint(net, name, op);
}

SOP_BreakingPoint::SOP_BreakingPoint(OP_Network *net, const char *name, OP_Operator *op)
	: SOP_Node(net, name, op)
{
    myCurrPoint = -1;	// To prevent garbage values from being returned
}

SOP_BreakingPoint::~SOP_BreakingPoint() {}

unsigned
SOP_BreakingPoint::disableParms()
{
    return 0;
}

OP_ERROR
SOP_BreakingPoint::cookMySop(OP_Context &context)
{
	fpreal		 now = context.getTime();

	// PUT YOUR CODE HERE
	// Declare the necessary variables and get always keep getting the current value in the node
	// For example to always get the current angle thats set in the node ,you need to :
	//    float angle;
	//    angle = ANGLE(now)       
    //    NOTE : ANGLE is a function that you need to use and it is declared in the header file to update your values instantly while cooking
	int pieces = PIECES(now);
	float scale = SCALE(now);
	int choice = CHOICE(now);
	float orix = ORIX(now);
	float oriy = ORIY(now);
	float oriz = ORIZ(now);

	float dirx = DIRX(now);
	float diry = DIRY(now);
	float dirz = DIRZ(now);

	UT_Vector3 origin(orix, oriy, oriz);
	UT_Vector3 direction(dirx, diry, dirz);

	std::cout << "ori: " << origin << std::endl;
	std::cout << "dir: " << direction << std::endl;

	std::vector<Geometry> meshes;

	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();

	if (choice == 0) {
		std::cout << "hi" << std::endl;

		// Check to see that there hasn't been a critical error in cooking the SOP.

		std::vector<Geometry> voroData = Voronoi::parseVoronoi("webFracture.txt", true);
		std::cout << "success??\n";
		for (int i = 0; i < voroData.size(); i++) {
				meshes.push_back(voroData.at(i));
		}
		std::cout << "success?\n";

	}

	if (choice == 1) {

		Viewport vp;
		//const GU_Detail *collision = inputGeo(0, context);
		GU_Detail * collision;
		duplicateSource(0, context);
		UT_Vector3 isect;
		Geometry cube = vp.testIntersect(gdp, isect, origin, direction);
		//std::cout << "it worked?" << std::endl;
		//std::cout << "x: " << isect[0] << " y: " << isect[1] << " z: " << isect[2] << std::endl;
		//meshes.push_back(cube);

		std::vector<float> isectVector = { isect[0], isect[1], isect[2] };
		Voronoi::createVoronoiFile(pieces, isectVector, scale, "voronoiOutput.txt");
		std::vector<Geometry> voroData = Voronoi::parseVoronoi("voronoiOutput.txt", true);
		std::vector<Geometry> splitMesh = std::vector<Geometry>();
		for (int i = 0; i < voroData.size(); i++) {
			igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_INTERSECT);
			std::cout << "entering testBoolean\n";
			Geometry outputGeometry = BooleanOps::testBoolean(cube, voroData.at(i), boolean_type);
			//std::cout << "Reached here\n";
			//std::cout << outputGeometry.first.size() << ", " << outputGeometry.second.size() << std::endl;
			if (outputGeometry.first.size() != 0 && outputGeometry.second.size() != 0) {
				//std::cout << "pushing outputGeometry\n";
				meshes.push_back(outputGeometry);
			}
		}
		if (scale < 0.5) {
			Voronoi::createVoronoiFile(1, isectVector, scale, "voronoiBoundsOutput.txt");
			std::vector<Geometry> voroBoundsData = Voronoi::parseVoronoi("voronoiBoundsOutput.txt", true);
			igl::MeshBooleanType booleanSub_type(igl::MESH_BOOLEAN_TYPE_MINUS);
			std::cout << "subtract\n";
			Geometry outputGeometry = BooleanOps::testBoolean(cube, voroBoundsData.at(0), booleanSub_type);
			if (outputGeometry.first.size() != 0 && outputGeometry.second.size() != 0) {
				//std::cout << "pushing outputGeometry\n";
				meshes.push_back(outputGeometry);
			}
			std::cout << "subtract success\n";
		}
	}

		/*std::cout << "intersected everything\n";
		Voronoi::createVoronoiFile(1, "cube.txt");
		std::vector<Geometry> cubeData = Voronoi::parseVoronoi("cube.txt");
		igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_MINUS);
		splitMesh.push_back(BooleanOps::testBoolean(cube, cubeData.at(0), boolean_type));
		std::cout << "subtracted\n";*/

		// PUT YOUR CODE HERE
		// Declare all the necessary variables
		UT_Interrupt	*boss;

		// Check to see that there hasn't been a critical error in cooking the SOP.
		if (error() < UT_ERROR_ABORT)
		{
			boss = UTgetInterrupt();
			gdp->clearAndDestroy();

			// Start the interrupt server
			if (boss->opStart("Running BreakingPoint"))
			{
				// PUT YOUR CODE HERE
				// Build a polygon
				// You need to build your cylinders inside Houdini from here
				// TIPS:
				// Use GU_PrimPoly poly = GU_PrimPoly::build(see what values it can take)
				// Also use GA_Offset ptoff = poly->getPointOffset()
				// and gdp->setPos3(ptoff,YOUR_POSITION_VECTOR) to build geometry.
				std::vector<GA_Offset> ptoffs;
				int offset = 0;
				for (int i = 0; i < meshes.size(); i++) {
					//std::cout << "offset: " << offset << std::endl;
					Points points = meshes[i].first;
					Faces faces = meshes[i].second;
					for (int j = 0; j < points.size(); j++) {
						std::vector<double> p = points[j];
						GA_Offset ptoff = gdp->appendPointOffset();
						gdp->setPos3(ptoff, UT_Vector3(p[0], p[1], p[2]));
						//std::cout <<"ptoff: " << ptoff << std::endl;
						ptoffs.push_back(ptoff);
					}
					for (int j = 0; j < faces.size(); j++) {
						GU_PrimPoly *tripoly = GU_PrimPoly::build(gdp, 3, GU_POLY_CLOSED);
						for (int idx = 0; idx < 3; idx++) {
							tripoly->setPointOffset(idx, ptoffs[faces[j][idx] + offset]);
						}
					}
					offset += points.size();
				}
				gdp->normal();
				////////////////////////////////////////////////////////////////////////////////////////////

				// Highlight the star which we have just generated.  This routine
				// call clears any currently highlighted geometry, and then it
				// highlights every primitive for this SOP. 
				select(GU_SPrimitive);
			}

			// Tell the interrupt server that we've completed. Must do this
			// regardless of what opStart() returns.
			boss->opEnd();
	}

    return error();
}