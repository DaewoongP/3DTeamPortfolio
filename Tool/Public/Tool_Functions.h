#pragma once

namespace Tool
{
	static bool VectorGetter(void* data, int idx, const char** out_str)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(data);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_str = vector.at(idx).c_str();
		return true;
	}
}
