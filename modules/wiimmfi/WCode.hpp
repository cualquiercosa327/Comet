#pragma once

#include <libpokey/hooks.h>
// Configuration
#define COMET_WCODE_REIMPLEMENTATION

#ifdef WCODE_CPP_INCLUDED_FROM_SOURCE
// Helpers local to this translation unit
#define RoundUpTo(x, n) (((u32)(x) + n - 1) & ~(n - 1))

#ifdef WIN32 // VS workaround
#define asm_ 
#else
#define asm_ asm
#endif

#define dcbf(p) __asm { dcbf 0, p; }
#define abs(x) ( (x) >= 0 ? (x) : -(x) )
#endif

// Copy and pasted as we can't include the headers from the private DWC decompilation.
// (C++ified, too)
namespace DWC {

struct AuthSvr
{
	enum _t {
		DEV, // Game development auth server
		RELEASE, // Release game auth server
		PROHIBITED // DWC library dev auth server
	};
};

extern AuthSvr::_t sAuthServer;

}
// Not otherwise publicly exposed
namespace SSL {

extern int l_initialized;

}

namespace Wiimmfi {


} // namespace Wiimmfi