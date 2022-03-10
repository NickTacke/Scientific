#pragma once
#include <cstdint>

class IClientRenderable;

class CDrawModelInfo
{
public:
	void* studioHdr;
	void* hardwareData;
	std::int32_t decals;
	std::int32_t skin;
	std::int32_t body;
	std::int32_t hitboxSet;
	IClientRenderable* renderable;
};