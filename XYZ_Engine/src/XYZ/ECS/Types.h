#pragma once
#include <bitset>

namespace XYZ {
#define MAX_ENTITIES 2^16
#define MAX_COMPONENTS 64
#define NULL_ENTITY -1

	using Entity = int32_t;
	using Signature = std::bitset<MAX_COMPONENTS>;
	using ComponentType = uint16_t;
}