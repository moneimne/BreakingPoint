#pragma once
#include <UI/UI_Event.h>
#include <GU/GU_RayPrimitive.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>

class Viewport
{
public:
	Viewport();
	~Viewport();
	void handleMouseEvent(UI_Event *event);
	void createRay();
	void testIntersect(GEO_Detail* collision);
};

