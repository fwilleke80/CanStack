#include "c4d.h"
#include "canstackgenerator.h"
#include "c4d_symbols.h"
#include "ostack.h"
#include "main.h"


// Unique ID obtained from www.plugincafe.com
const Int32 ID_STACK = 1038758;


class StackObject : public ObjectData
{
	INSTANCEOF(StackObject, ObjectData)
	
public:
	virtual Bool Init(GeListNode *node);
	virtual Bool Message(GeListNode *node, Int32 type, void *t_data);
	
	virtual Bool GetDEnabling(GeListNode *node, const DescID &id, const GeData &t_data, DESCFLAGS_ENABLE flags, const BaseContainer *itemdesc);

	virtual BaseObject* GetVirtualObjects(BaseObject *op, HierarchyHelp *hh);

	static NodeData* Alloc()
	{
		return NewObjClear(StackObject);
	}
	
private:
	CanStackGenerator	_stackGenerator;
};


// Initialize settings
Bool StackObject::Init(GeListNode *node)
{
	// Good practice: Check for nullptr
	if (!node)
		return false;
	
	BaseObject *op	= static_cast<BaseObject*>(node);
	BaseContainer *data = op->GetDataInstance();

	data->SetInt32(STACK_BASE_COUNT, 3);
	data->SetFloat(STACK_BASE_LENGTH, 100.0);
	data->SetInt32(STACK_ROWS_COUNT, 3);
	data->SetFloat(STACK_ROWS_HEIGHT, 20.0);
	data->SetUInt32(STACK_RANDOM_SEED, 12345);
	data->SetFloat(STACK_RANDOM_POS, 0.0);
	data->SetFloat(STACK_RANDOM_ROT, 0.0);

	return SUPER::Init(node);
}


// Catch messages
Bool StackObject::Message(GeListNode *node, Int32 type, void *data)
{
	// Good practice: Check for nullptr
	if (!node)
		return false;
	
	switch (type)
	{
		// Description validation: Make sure STACK_ROWS_COUNT doesn't get higher than possible
		case MSG_DESCRIPTION_VALIDATE:
		{
			BaseContainer* bc = static_cast<BaseObject*>(node)->GetDataInstance();
			
			Int32 baseCount = bc->GetInt32(STACK_BASE_COUNT, 0);
			Int32 maxRows = bc->GetInt32(STACK_ROWS_COUNT);
			bc->SetInt32(STACK_ROWS_COUNT, Min(maxRows, baseCount));
			
			break;
		}
	
		// Command button pressed
		case MSG_DESCRIPTION_COMMAND:
		{
			// Good practice: Check for nullptr when valid pointer required
			if (!data)
				return false;
			
			// Get message data
			DescriptionCommand *dc = (DescriptionCommand*)data;
			
			// Fit STACK_ROWS_HEIGHT to height of child object
			if (dc->id == STACK_CMD_FITHEIGHT)
			{
				// Get child object
				BaseObject *child = static_cast<BaseObject*>(node->GetDown());
				if (child)
				{
					// Get child's bounding box radius
					Vector rad = child->GetRad();

					// If radius is valid
					if (rad.IsNotZero())
					{
						// Get Container
						BaseContainer* bc = static_cast<BaseObject*>(node)->GetDataInstance();
						
						// Set STACK_ROWS_HEIGHT to radius*2
						bc->SetFloat(STACK_ROWS_HEIGHT, rad.y * 2.0);
					}
				}
			}
			
			break;
		}
			
	}
	
	return SUPER::Message(node, type, data);
}


// Grey out unused attributes
Bool StackObject::GetDEnabling(GeListNode *node, const DescID &id, const GeData &t_data, DESCFLAGS_ENABLE flags, const BaseContainer *itemdesc)
{
	// Good practice: Check for nullptr
	if (!node)
		return false;
	
	BaseObject *op = static_cast<BaseObject*>(node);
	BaseContainer *bc = op->GetDataInstance();
	
	switch (id[0].id)
	{
		// Disable length attribute is a path spline is used
		case STACK_BASE_LENGTH:
			return !bc->GetObjectLink(STACK_BASE_PATH, op->GetDocument());
	}
	
	return SUPER::GetDEnabling(node, id, t_data, flags, itemdesc);
}


// Generate stack
BaseObject* StackObject::GetVirtualObjects(BaseObject *op, HierarchyHelp *hh)
{
	// Good practice: Check for nullptr
	if (!op || !hh)
		return nullptr;
	
	// Get container
	BaseContainer *bc = op->GetDataInstance();
	
	// Get document
	BaseDocument *doc = op->GetDocument();
	if (!doc)
		return nullptr;
	
	// Get child object for cloning
	BaseObject *child = op->GetDown();
	if (!child)
		return nullptr;

	// Get clone of child object
	Bool dirty = false;
	BaseObject *childClone = op->GetAndCheckHierarchyClone(hh, child, HIERARCHYCLONEFLAGS_0, &dirty, nullptr, false);
	if (!childClone)
		return nullptr;
	
	// Return cache if already built
	if (!dirty)
		return childClone;
	
	// Get stack parameters from container
	StackParameters params;
	params._baseCount = bc->GetInt32(STACK_BASE_COUNT);
	params._baseLength = bc->GetFloat(STACK_BASE_LENGTH);
	params._rowHeight = bc->GetFloat(STACK_ROWS_HEIGHT);
	params._rowCount = bc->GetInt32(STACK_ROWS_COUNT);
	params._randomSeed = bc->GetInt32(STACK_RANDOM_SEED);
	params._randomPos = bc->GetFloat(STACK_RANDOM_POS);
	params._randomRot = bc->GetFloat(STACK_RANDOM_ROT);
	params._basePath = static_cast<SplineObject*>(bc->GetObjectLink(STACK_BASE_PATH, doc));
	
	// Initialize stack
	if (!_stackGenerator.InitStack(params))
		return nullptr;
	
	// Generate stack item
	if (!_stackGenerator.GenerateStack())
		return nullptr;
	
	// Build geometry
	BaseObject *result = _stackGenerator.BuildStackGeometry(childClone, op->GetMg());
	if (!result)
		return nullptr;
	
	// Name parent result object
	result->SetName(GeLoadString(IDS_STACK));
	
	return result;
}


// Register object plugin
Bool RegisterStackObject()
{
	return RegisterObjectPlugin(ID_STACK, GeLoadString(IDS_STACK), OBJECT_GENERATOR|OBJECT_INPUT, StackObject::Alloc, "Ostack", AutoBitmap("ostack"), 0);
}
