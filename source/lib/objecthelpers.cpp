#include "objecthelpers.h"


BaseObject *GetCurrentStateToObject(BaseObject *inputObject, Int32 &nodeType)
{
	// Create AliasTranslate
	AutoAlloc<AliasTrans> aliasTrans;
	if (!aliasTrans || !aliasTrans->Init(inputObject->GetDocument()))
		return nullptr;
	
	// Create clone of op. We only need this for the modeling command.
	BaseObject *tmpOp = static_cast<BaseObject*>(inputObject->GetClone(COPYFLAGS_0, aliasTrans));
	if (!tmpOp)
		return nullptr;
	
	// Translate BaseLinks, maybe the cloned object needs that
	aliasTrans->Translate(true);
	
	// Create temporary document
	AutoAlloc<BaseDocument> tmpDoc;
	if (!tmpDoc)
	{
		// Free tmpOp and return
		BaseObject::Free(tmpOp);
		return nullptr;
	}
	
	// Insert tmpOp into tmpDoc. From now on, tmpDoc has the ownership over tmpOp,
	// so we don't need to free tmpOp manually anymore (because tmpDoc will auto-free itself at the end of the scope, as we used AutoAlloc to create it).
	tmpDoc->InsertObject(tmpOp, nullptr, nullptr);
	
	// Build modeling command data
	ModelingCommandData mcd;
	mcd.doc = tmpDoc;
	mcd.op = tmpOp;
	
	// Perform modeling command
	if (!SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, mcd))
		return nullptr;
	
	// Get result
	BaseObject *resultObject = static_cast<BaseObject*>(mcd.result->GetIndex(0));
	
	// Set original matrix
	if (resultObject)
		resultObject->SetMg(inputObject->GetMg());
	
	// Set type of result object
	nodeType = resultObject->GetType();

	// Return result object
	return resultObject;
}


MinMax CalculateBoundingBox(BaseObject *inputObject)
{
	// Good practice: Check for nullptr
	if (!inputObject)
		return MinMax();
	
	// Cancel if object is neither polygon nor spline
	Int32 inputType = inputObject->GetType();
	if (!(inputType == Opolygon || inputType == Ospline))
		return MinMax();
	
	// Cast to PointObject
	PointObject *pointObject = static_cast<PointObject*>(inputObject);
	
	// Initialize bounding box
	MinMax boundingBox;
	boundingBox.Init();
	
	// Get read-only points array and point count
	Int32 pointCount = pointObject->GetPointCount();
	const Vector *padr = pointObject->GetPointR();
	if (!padr)
		return MinMax();
	
	// Iterate points and extend bounding box
	for (Int32 i = 0; i < pointCount; i++)
	{
		boundingBox.AddPoint(padr[i]);
	}
	
	// Return bounding box
	return boundingBox;
}


MinMax CalculateHierarchyBoundingBox(BaseObject *inputObject)
{
	MinMax boundingBox;

	// Iterate objects horizontally
	while (inputObject)
	{
		// Get bounding box & add to existing data
		MinMax tmpBoundingBox = CalculateBoundingBox(inputObject);
		boundingBox.AddPoints(tmpBoundingBox.GetMin(), tmpBoundingBox.GetMax());
		
		// Recurse & add result to existing data
		tmpBoundingBox = CalculateHierarchyBoundingBox(inputObject->GetDown());
		boundingBox.AddPoints(tmpBoundingBox.GetMin(), tmpBoundingBox.GetMax());
		
		// Continue with next object
		inputObject = inputObject->GetNext();
	}
	
	// Return bounding box
	return boundingBox;
}


void TouchAllChildren(BaseObject *startObject)
{
	// Cancel if no object
	if (!startObject)
		return;
	
	// Get child object
	BaseObject *childObject = startObject->GetDown();
	
	// Touch child object
	while (childObject)
	{
		// Touch all children of child object
		TouchAllChildren(childObject);

		// Touch child object itself
		childObject->Touch();
		
		// Continue with next object
		childObject = childObject->GetNext();
	}
}


Bool IsDirtyChildren(BaseObject *startObject, DIRTYFLAGS flags)
{
	// Cancel if no object
	if (!startObject)
		return false;
	
	// Get child object
	BaseObject *childObject = startObject->GetDown();

	// Ask children for dirtyness
	Bool dirty = false;
	
	// Return IsDirty() of child object and its children
	while (childObject)
	{
		// Ask children of child object for their dirtyness
		dirty |= IsDirtyChildren(childObject, flags);
		
		// Ask child object for dirtyness
		dirty |= childObject->IsDirty(flags);
		
		// Continue with next object
		childObject = childObject->GetNext();
	}
	
	return dirty;
}
