#pragma once

#if AC_ENABLE_TRACY
	#include "Tracy.hpp"
#else
	#define ZoneScopedN(...)
	#define ZoneTransientN(...)
	#define ZoneScoped
#endif
