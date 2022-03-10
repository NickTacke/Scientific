#pragma once
#include "../Classes/CVector.h"
#include "IEngineTraceClient.h"

class IClientRenderable;

struct RenderableInfo_t {
	IClientRenderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;   // 0x0016
	uint16_t m_Flags2; // 0x0018
	CVector m_vecBloatedAbsMins;
	CVector m_vecBloatedAbsMaxs;
	CVector m_vecAbsMins;
	CVector m_vecAbsMaxs;
	int pad;
};

class SpatialLeafEnumerator
{
public:
	// call back with a leaf and a context
	// The context is completely user defined; it's passed into the enumeration
	// function of ISpatialQuery.
	// This gets called	by the enumeration methods with each leaf
	// that passes the test; return true to continue enumerating,
	// false to stop

	virtual bool EnumerateLeaf(int leaf, int* context) = 0;
};

class SpatialQuery
{
public:
	// Returns the number of leaves
	virtual int LeafCount() const = 0;

	// Enumerates the leaves along a ray, box, etc.
	virtual bool EnumerateLeavesAtPoint(const CVector& pt, SpatialLeafEnumerator* pEnum, int* context) = 0;
	virtual bool EnumerateLeavesInBox(const CVector& mins, const CVector& maxs, SpatialLeafEnumerator* enumumerator, int* context) = 0;
	virtual bool EnumerateLeavesInSphere(const CVector& center, float radius, SpatialLeafEnumerator* enumumerator, int* context) = 0;
	virtual bool EnumerateLeavesAlongRay(CRay const& ray, SpatialLeafEnumerator* pEnum, int* context) = 0;
	virtual bool EnumerateLeavesInSphereWithFlagSet(const CVector& center, float radius, SpatialLeafEnumerator* enumumerator, int* context, int flagsCheck) = 0;
	virtual int ListLeavesInBox(const CVector& mins, const CVector& maxs, unsigned short* list, int listMax) = 0;

	// Used to determine which leaves passed in (specified by leafcount, pLeafs, and nLeafStride )
	// are within radius flRadius of vecCenter and have the flag set.
	// The result is placed in the pLeafsInSphere array, which specifies _indices_ into the original pLeafs array
	// The number of leaves found within the sphere is the return value.
	// The caller is expected to have allocated at least nLeafCount pLeafsInSphere to place the results into
	virtual int ListLeavesInSphereWithFlagSet(int* leafsInSphere, const CVector& center, float radius, int leafCount, const uint16_t* leafs, int leafStride = sizeof(uint16_t), int flagsCheck = 0xFFFFFFFF) = 0;
};