
#include "unicorn_library.h"

namespace u {
	extern const Library lib_const;
	extern const Library lib_standard;
	extern const Library lib_linecode;

	const Library libraries[] = { lib_const, lib_standard, lib_linecode };
	const size_t libraries_count = sizeof(libraries) / sizeof(libraries[0]);
}