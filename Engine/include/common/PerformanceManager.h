#pragma once
#include <string>

#define PROFILE_FUNCTION() Performance::Profiler s_sect(__FUNCTION__);
#define PROFILE_SECTION(id) Performance::Profiler s_sect(id);

namespace Performance
{
	class Profiler
	{
	public:
		explicit Profiler(const std::string& inName);
		~Profiler();
	private:
		std::string m_name;
	};

	void StartTimer(const std::string& inName);
	void EndTimer(const std::string& inName);
	void DebugImgui(bool& inShow);
}
