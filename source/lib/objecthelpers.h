#ifndef WS_BOUNDINGBOX_H__
#define WS_BOUNDINGBOX_H__


#include "c4d.h"


/// Performs MCOMMAND_CURRENTSTATETOOBJECT on an object and
/// returns the result.
/// @param[in] inputObject The source object
/// @param[out] nodeType Returns the type (plugin ID) of the resulting object
/// @return The resulting object, or nullptr if an error occurred. Caller owns the pointed object.
BaseObject *GetCurrentStateToObject(BaseObject *inputObject, Int32 &nodeType);

/// Calculates the bounding box
/// @param[in] inputObject The PointObject to calculate the bounding box from
/// @return The bounding box in object space
MinMax CalculateBoundingBox(BaseObject *inputObject);

/// Recursively touch all child objects of an object
/// @param[in] startObject The parent object of the hierarchy that should be touched. All child objects (not startObject itself!) will be touched.
void TouchAllChildren(BaseObject *startObject);

/// Recursively call IsDirty() on all child objects under 'startObject'
/// @param[in] startObject The parent object of the hierarchy that should be checked for dirtyness. All child objects (not startObject itself!) will be touched.
/// @param[in] flags DIRTYFLAGS bitmask to use for IsDirty() calls
/// @return True if any of the child objects is dirty, otherwise false.
Bool IsDirtyChildren(BaseObject *startObject, DIRTYFLAGS flags);


#endif // WS_BOUNDINGBOX_H__
