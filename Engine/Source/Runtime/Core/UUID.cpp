#include "Xpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace X
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		: mUUID(s_UniformDistribution(s_Engine))
	{
	}

	UUID::UUID(uint64_t uuid)
		: mUUID(uuid)
	{
	}

	UUID::UUID(const UUID& other)
		: mUUID(other.mUUID)
	{
	}

}
