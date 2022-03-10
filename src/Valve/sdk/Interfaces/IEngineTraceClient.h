#pragma once
#include "../../../Utils/Memory.h"

#include "../Classes/CBaseEntity.h"
#include "../Classes/CVector.h"

class CRay
{
public:
	constexpr CRay(const CVector& src, const CVector& dst) noexcept
	{
		start = src;
		delta = dst - src;
		isSwept = delta.x || delta.y || delta.z;
	}

private:
	CVector start;
	char pad0[4]{ };
	CVector delta;
	char pad1[40]{ };
	bool isRay{ true };
	bool isSwept;
};

class CTraceFilter
{
public:
	constexpr CTraceFilter(const CBaseEntity* entity) noexcept
	{
		skip = entity;
	}

	virtual bool ShouldHit(CBaseEntity* entity, std::int32_t mask)
	{
		return entity != skip;
	}

	virtual std::int32_t GetTraceType() const
	{
		return 0;
	}

	const CBaseEntity* skip;
};

// I know this differs slightly from the one I used in the
// video... this one is more accurate :)
class CTrace
{
public:
	CVector start;
	CVector end;
	char pad[20];
	float fraction;
	int contents;
	unsigned short dispFlags;
	bool allSolid;
	bool startSolid;
	char pad1[4];
	struct Surface_t {
		const char* name;
		short surfaceProps;
		unsigned short flags;
	} surface;
	int hitgroup;
	char pad2[4];
	CBaseEntity* entity;
};

class IEngineTraceClient
{
public:
	constexpr void TraceRay(
		const CRay& ray,
		const std::uintptr_t mask,
		const CTraceFilter& filter,
		CTrace& trace) noexcept
	{
		return Memory::vmt::Call<void, const CRay&, const std::uintptr_t, const CTraceFilter&, CTrace&>(
			this,
			5,
			ray,
			mask,
			filter,
			trace
			);
	}
};