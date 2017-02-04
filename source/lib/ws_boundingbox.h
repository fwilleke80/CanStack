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
/// @param[in] op The PointObject to calculate the bounding box from
/// @return The bounding box in object space
MinMax CalculateBoundingBox(BaseObject *op);


#endif // WS_BOUNDINGBOX_H__
