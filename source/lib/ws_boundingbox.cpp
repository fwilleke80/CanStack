#include "ws_boundingbox.h"

BaseObject *GetCurrentStateToObject(BaseObject *op)
{
	// Create AliasTranslate
	AutoAlloc<AliasTrans> aliasTrans;
	if (!aliasTrans || !aliasTrans->Init(op->GetDocument()))
		return nullptr;
	
	// Create clone of op. We only need this for the modeling command.
	BaseObject *tmpOp = static_cast<BaseObject*>(op->GetClone(COPYFLAGS_0, aliasTrans));
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
	BaseObject *res = static_cast<BaseObject*>(mcd.result->GetIndex(0));
	
	// Set original matrix
	if (res)
		res->SetMg(op->GetMg());

	return res;
}


MinMax CalculateBoundingBox(BaseObject *op)
{
	if (!op)
		return MinMax();

	PolygonObject *polyObj = static_cast<PolygonObject*>(GetCurrentStateToObject(op));
	if (!polyObj)
		return MinMax();

	MinMax boundingBox;
	boundingBox.Init();

	const Vector *padr = polyObj->GetPointR();
	Int32 pointCount = polyObj->GetPointCount();
	
	for (Int32 i = 0; i < pointCount; i++)
	{
		boundingBox.AddPoint(padr[i]);
	}
	
	// TODO: Remove debug print
	//GePrint("Bounding box of " + op->GetName() + "(" + polyObj->GetName() + "): Mp=" + String::VectorToString(boundingBox.GetMp()) + "; Rad=" + String::VectorToString(boundingBox.GetRad()));
	
	return boundingBox;
}
