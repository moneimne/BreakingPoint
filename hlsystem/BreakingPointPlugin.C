


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
static PRM_Name forceName("force", "Impact Force");
static PRM_Name piecesName("pieces", "Number of Pieces");

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				     ^^^^^^^^    ^^^^^^^^^^^^^^^
//				     internal    descriptive version


// PUT YOUR CODE HERE
// You need to setup the initial/default values for your parameters here
// For example : If you are declaring the inital value for the angle parameter
// static PRM_Default angleDefault(30.0);	

static PRM_Default forceDefault(30.0);
static PRM_Range forceRange(PRM_RANGE_UI, 0.0, PRM_RANGE_UI, 500.0);
static PRM_Default piecesDefault(1);


////////////////////////////////////////////////////////////////////////////////////////

PRM_Template
SOP_BreakingPoint::myTemplateList[] = {
	// PUT YOUR CODE HERE
	// You now need to fill this template with your parameter name and their default value
	// EXAMPLE : For the angle parameter this is how you should add into the template
	// PRM_Template(PRM_FLT,	PRM_Template::PRM_EXPORT_MIN, 1, &angleName, &angleDefault, 0),
	// Similarly add all the other parameters in the template format here
		PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &forceName, &forceDefault, 0, &forceRange),
		PRM_Template(PRM_INT, PRM_Template::PRM_EXPORT_MIN, 1, &piecesName, &piecesDefault, 0),

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
	LSystem myplant;
	float force = FORCE(now);
	int pieces = PIECES(now);

	Viewport vp;
	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT)
		return error();
	//const GU_Detail *collision = inputGeo(0, context);
	GU_Detail * collision;
	duplicateSource(0, context);
	UT_Vector3 isect;
	Geometry cube = vp.testIntersect(gdp,isect);
	std::cout << "it worked?" << std::endl;
	std::cout << "x: " << isect[0] << " y: " << isect[1] << " z: " << isect[2] << std::endl;

	Voronoi::createVoronoiFile(pieces, "voronoiOutput.txt");
	std::vector<Geometry> voroData = Voronoi::parseVoronoi("voronoiOutput.txt");
	std::vector<Geometry> splitMesh = std::vector<Geometry>();
	for (int i = 0; i < voroData.size(); i++) {
		// TODO: Offset voronoi data
		igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_INTERSECT);
		Geometry outputGeometry = BooleanOps::testBoolean(cube, voroData.at(i), boolean_type);
		if (outputGeometry.first.size() != 0 && outputGeometry.second.size() != 0) {
			splitMesh.push_back(outputGeometry);
		}
	}
	/*std::cout << "intersected everything\n";
	Voronoi::createVoronoiFile(1, "cube.txt");
	std::vector<Geometry> cubeData = Voronoi::parseVoronoi("cube.txt");
	igl::MeshBooleanType boolean_type(igl::MESH_BOOLEAN_TYPE_MINUS);
	splitMesh.push_back(BooleanOps::testBoolean(cube, cubeData.at(0), boolean_type));
	std::cout << "subtracted\n";*/


	///////////////////////////////////////////////////////////////////////////

	//PUT YOUR CODE HERE
	// Next you need to call your Lystem cpp functions 
	// Below is an example , you need to call the same functions based on the variables you declare
    // myplant.loadProgramFromString("F\nF->F[+F]F[-F]";  
    // myplant.setDefaultAngle(30.0f);
    // myplant.setDefaultStep(1.0f);
	/*myplant.loadProgramFromString(grammar.toStdString());
	myplant.setDefaultAngle(angle);
	myplant.setDefaultStep(step);*/
	


	///////////////////////////////////////////////////////////////////////////////

	// PUT YOUR CODE HERE
	// You the need call the below function for all the genrations ,so that the end points points will be
	// stored in the branches vector , you need to declare them first

	/*std::vector<std::pair<vec3, vec3>> branches;
	myplant.process(iterations, branches);*/
	/*for (int i = 0; i < iterations; i++)
	{
		  myplant.process(i, branches);
	}*/





	///////////////////////////////////////////////////////////////////////////////////


	// Now that you have all the branches ,which is the start and end point of each point ,its time to render 
	// these branches into Houdini 
    

	// PUT YOUR CODE HERE
	// Declare all the necessary variables for drawing cylinders for each branch 
    float		 rad, tx, ty, tz;
    int			 divisions, plane;
    int			 xcoord =0, ycoord = 1, zcoord =2;
    float		 tmp;
    UT_Vector4		 pos;
    GU_PrimPoly		*poly;
    int			 i;
    UT_Interrupt	*boss;
	vec3 start, end;

    // Since we don't have inputs, we don't need to lock them.

    divisions  = 5;	// We need twice our divisions of points
    myTotalPoints = divisions;		// Set the NPT local variable value
    myCurrPoint   = 0;			// Initialize the PT local variable



    // Check to see that there hasn't been a critical error in cooking the SOP.
    if (error() < UT_ERROR_ABORT)
    {
	boss = UTgetInterrupt();
	if (divisions < 4)
	{
	    // With the range restriction we have on the divisions, this
	    //	is actually impossible, but it shows how to add an error
	    //	message or warning to the SOP.
	    addWarning(SOP_MESSAGE, "Invalid divisions");
	    divisions = 4;
	}
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

		/*for (int i = 0; i < branches.size(); i++) {
			vec3 start = branches.at(i).first;
			vec3 end = branches.at(i).second;

			poly = GU_PrimPoly::build(gdp, 2, GU_POLY_OPEN);
			
			GA_Offset ptoff1 = poly->getPointOffset(0);
			gdp->setPos3(ptoff1, UT_Vector3(start[0], start[1], start[2]));
			
			GA_Offset ptoff2 = poly->getPointOffset(1);
			gdp->setPos3(ptoff2, UT_Vector3(end[0], end[1], end[2]));
		}*/




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

    myCurrPoint = -1;
    return error();
}

