
#ifndef UNICORN_LIBRARY_H_GUARD
#define UNICORN_LIBRARY_H_GUARD

#include "unicorn_cfg.h"
#include "unicorn_block.h"

//TODO: is this file really needed?
namespace u
{
	struct Library {
		const char* name;
		size_t count;
		const Block *blocks;
	};

	type::n* work_dummy(port** portlist);
	NodeTuneResult tune_dummy(Node* node, NodeTuneType tune_type);
	extern const char* string_dummy;

	extern const Library lib_const;
	extern const Library lib_standard;
	extern const Library lib_linecode;

	extern const Library libraries[];
	extern const size_t libraries_count;
}
#endif
