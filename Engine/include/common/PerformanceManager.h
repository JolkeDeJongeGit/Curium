#pragma once
#include <string>

#define PROFILE_FUNCTION() Performance::Profiler s_sect(__FUNCTION__);
#define PROFILE_SECTION(id) Performance::Profiler s_sect(id);

namespace Performance
{
	class Profiler
	{
	public:
		Profiler(const std::string& name);
		~Profiler();
	private:
		std::string m_name;
	};

	void StartTimer(const std::string& name);
	void EndTimer(const std::string& name);
	void DebugImgui(bool& show);
}
