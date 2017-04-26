
#pragma once

#include <stdint.h>

// TODO: The ScenePainter UnrolledGraphicsAlgorithms has some newer versions where,
//       in particular, the strafe is passed in *bytes* to avoid runtime C-pointer-arithmetic.

template<typename Item>
Item *ArrayFill(
				Item *destinationAddress,
				Item  fillerValue,
				uintptr_t  itemCount)
{
	// Fills an array of memory with the given value.
	// Returns the address (destinationAddress + itemCount)
	auto d = destinationAddress;
	auto e = destinationAddress + itemCount;
	while(d != e) *d++ = fillerValue;
	return e;
}




template<typename Item>
Item *ArrayFill2D(
				  Item *destinationAddress,         // destinationAddress is the first entry to write to.
				  Item  fillerValue,
				  uintptr_t  itemCountX,          // itemCountX is the number of contiguous entries to write to.
				  uintptr_t  itemCountY,          // itemCountY is the number of 'lines' to write to.
				  intptr_t  pointerDeltaCount)    // PtrDelta is the 'step' value (number of ITEMS -- not bytes!)
{
	// Fills a 2D array with the given value.
	// Returns the address (destinationAddress + (itemCountY * PtrDelta))
	while(itemCountY)
	{
		ArrayFill(destinationAddress, fillerValue, itemCountX);
		destinationAddress += pointerDeltaCount;
		itemCountY--;
	}
	return destinationAddress;
}




template<typename Item>
intptr_t ArrayCompare(
					 const Item *area1,
					 const Item *area2,
					 uintptr_t itemCount)
{
	// Compares two arrays (entry for entry) with operator==
	// Returns 1=Identical (operator== returned true for all), 0=not equal
	auto d = area1;
	auto e = area1 + itemCount;
	auto s = area2;
	while(d != e)
	{
		if(*d != *s) return 0;
		d++;
		s++;
	}
	return 1;
}




template<typename Item>
Item *ArrayCopy(
				Item *destinationAddress,
				const Item *sourceAddress,
				uintptr_t itemCount)
{
	// Copies array from source to destination.
	// Returns destinationAddress + itemCount.
	auto d = destinationAddress;
	auto e = destinationAddress + itemCount;
	auto s = sourceAddress;
	while(d != e) *d++ = *s++;
	return d;
}




template<typename Item>
Item *ArrayCopy2D(
				  Item *destinationAddress,
				  const Item *sourceAddress,
				  uintptr_t itemCountX,
				  uintptr_t itemCountY,
				  intptr_t destinationPointerDelta,
				  intptr_t sourcePointerDelta)
{
	while(itemCountY)
	{
		ArrayCopy( destinationAddress, sourceAddress, itemCountX );
		destinationAddress += destinationPointerDelta;
		sourceAddress      += sourcePointerDelta;
		itemCountY--;
	}
	return destinationAddress;
}




template<typename Item>
Item *TransparentArrayCopy(
						   Item *destinationAddress,
						   const Item *sourceAddress,
						   uintptr_t itemCount,
						   const Item missValue)
{
	// Copies array from source to destination,
	// except for slots with a particular value.
	// Returns destinationAddress + itemCount.
	auto d = destinationAddress;
	auto e = destinationAddress + itemCount;
	auto s = sourceAddress;
	while(d != e)
	{
		if(*s != missValue) *d = *s;
		d++;
		s++;
	}
	return d;
}




template<typename Item>
Item *TransparentArrayCopy2D(
							 Item *destinationAddress,
							 const Item *sourceAddress,
							 uintptr_t  itemCountX,
							 uintptr_t  itemCountY,
							 intptr_t  destinationPointerDelta,
							 intptr_t  sourcePointerDelta,
							 Item  missValue)
{
	// TransparentArrayCopy2D
	while(itemCountY)
	{
		TransparentArrayCopy( destinationAddress, sourceAddress, itemCountX, missValue );
		destinationAddress += destinationPointerDelta;
		sourceAddress      += sourcePointerDelta;
		itemCountY--;
	}
	return destinationAddress;
}





template<typename Item>
Item *ArrayAverageFill(
						 Item *destinationAddress,
						 Item  fillerValue,
						 Item  ValueMask,    // eg: 0xFEFEFE
						 Item  ValueShift,   // eg: 1
						 uintptr_t  itemCount)
{
	fillerValue = (fillerValue & ValueMask) >> ValueShift;
	auto d = destinationAddress;
	auto e = destinationAddress + itemCount;
	while(d != e)
	{
		*d = (((*d) & ValueMask) >> ValueShift) + fillerValue;
		++d;
	}
	return e;
}




template<typename Item>
Item *ArrayAverageFill2D(
						 Item *destinationAddress,
						 Item  fillerValue,
						 Item  valueMask,    // eg: 0xFEFEFE
						 Item  valueShift,   // eg: 1
						 uintptr_t  itemCountX,
						 uintptr_t  itemCountY,
						 intptr_t   pointerDeltaCount)
{
	// Fills a 2D array with the given value.
	// destinationAddress is the first entry to write to.
	// itemCountX is the number of contiguous entries to write to.
	// itemCountY is the number of 'lines' to write to.
	// PtrDelta is the 'step' value (number of ITEMS -- not bytes!)
	// Returns the address (destinationAddress + (itemCountY * PtrDelta))
	while(itemCountY)
	{
		ArrayAverageFill(destinationAddress, fillerValue, valueMask, valueShift, itemCountX);
		destinationAddress += pointerDeltaCount;
		itemCountY--;
	}
	return destinationAddress;
}


