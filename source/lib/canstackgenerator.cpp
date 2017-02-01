#include "canstackgenerator.h"


Bool CanStackGenerator::InitStack(const StackParameters &params)
{
	// Init random number generator (we have to do this always to ensure reproducible random results!)
	_random.Init(params._randomSeed);

	// If new params are the same as the previous ones, don't do anything else
	if (params == _params)
		return true;
	
	// Default member values
	_params = StackParameters();
	_initialized = false;
	
	// Cancel if nonsense baseCount
	if (params._baseCount < 1)
		return false;
	
	// Store parameters internally
	_params = params;
	
	// Make sure the stack array and its row arrays are of correct size
	if (!ResizeStack(_params._baseCount, _params._rowCount))
		return false;
	
	// Success, we made it!
	_initialized = true;
	return _initialized;
}


Bool CanStackGenerator::GenerateStack()
{
	if (!_initialized)
		return false;
	
	// Some values
	Float distance(0.0);			// Distance between items in a normal row
	Float relDistance(0.0);		// Relative distance between items in a row on a path spline
	Matrix splineMg;
	
	// If spline is used, use length of spline as baseLength
	if (_params._basePath)
	{
		splineMg = _params._basePath->GetMg();
		
		// Allocate SplineHelp
		if (!_splineLengthData)
		{
			_splineLengthData.Set(SplineLengthData::Alloc());
			if (!_splineLengthData)
				return false;
		}
		
		// Initialize SplineHelp
		if (!_splineLengthData->Init(_params._basePath))
			return false;

		relDistance = 1.0 / (Float)(_params._baseCount - 1);
	}
	else
	{
		distance = _params._baseLength / (Float)_params._baseCount;
	}

	// Iterate stack rows
	Int32 rowIndex = 0;
	for (StackRowArray::Iterator row = _array.Begin(); row != _array.End(); ++row, rowIndex++)
	{
		// Iterate items in row
		// Create positions for current row
		Int32 itemIndex = 0;
		for (MatrixArray::Iterator item = row->Begin(); item != row->End(); ++item, itemIndex++)
		{
			// Compute rotation matrix & set to item
			*item = HPBToMatrix(Vector(_random.Get11() * _params._randomRot, 0.0, 0.0), ROTATIONORDER_HPB);
			
			// Compute matrix offset
			if (_params._basePath)
			{
				// Calculate item's relative offset on the spline
				Float relOffset = _splineLengthData->UniformToNatural((Float)itemIndex * relDistance);

				// Calculate position along spline
				item->off = _params._basePath->GetSplinePoint(relOffset) + Vector(_random.Get11() * _params._randomPos, _params._rowHeight * rowIndex, _random.Get11() * _params._randomPos);
				*item = splineMg * *item;
			}
			else
			{
				// Calculate item's position
				item->off = Vector(_random.Get11() * _params._randomPos, _params._rowHeight * rowIndex, distance * itemIndex + distance * rowIndex * 0.5 + _random.Get11() * _params._randomPos);
			}
		}
	}
	
	return true;
}


BaseObject *CanStackGenerator::BuildStackGeometry(BaseObject *originalObject, const Matrix &mg)
{
	// Create parent object
	AutoAlloc<BaseObject> resultParent(Onull);
	if (!resultParent)
		return nullptr;
	
	// Iterate rows in stack
	for (StackRowArray::Iterator row = _array.Begin(); row != _array.End(); ++row)
	{
		// Iterate items in row
		for (MatrixArray::Iterator item = row->Begin(); item != row->End(); ++item)
		{
			// Create clone of original object
			BaseObject *clone = static_cast<BaseObject*>(originalObject->GetClone(COPYFLAGS_0, nullptr));
			if (!clone)
				return nullptr;
			
			// Set clone position according to item in stack data
			if (_params._basePath)
				clone->SetMg(*item);
			else
				clone->SetMl(*item);
			
			// Insert clone as last child under parent Null
			clone->InsertUnderLast(resultParent);
		}
	}
	
	// Return parent Null and give up ownership
	return resultParent.Release();
}


Bool CanStackGenerator::ResizeStack(Int32 baseCount, Int32 rowCount)
{
	// Resize stack
	if (!_array.Resize(Min(baseCount, rowCount)))
		return false;
	
	// Resize rows in stack
	Int32 rowIndex = 0;
	for (StackRowArray::Iterator row = _array.Begin(); row != _array.End() && rowIndex < rowCount; ++row, rowIndex++)
	{
		// Each row is 1 smaller than its predecessor
		if (!row->Resize(_params._baseCount - rowIndex))
			return false;
	}
	
	return true;
}
