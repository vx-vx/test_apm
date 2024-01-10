#pragma once
#include <map>

namespace gl_params
{
	extern std::map<int, time_t> apm_map;
	extern std::map<int, time_t> cps_map;

	extern int count;
	extern int count2;
	extern bool eject;
	extern int apm;
	extern time_t cur_time;
	extern int cps;
}