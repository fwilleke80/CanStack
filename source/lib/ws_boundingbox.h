#ifndef WS_BOUNDINGBOX_H__
#define WS_BOUNDINGBOX_H__


#include "c4d.h"

BaseObject *GetCurrentStateToObject(BaseObject *op);
MinMax CalculateBoundingBox(BaseObject *op);


#endif // WS_BOUNDINGBOX_H__
