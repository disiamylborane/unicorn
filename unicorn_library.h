
#ifndef UNICORN_LIBRARY_H_GUARD
#define UNICORN_LIBRARY_H_GUARD

#include "unicorn_cfg.h"
#include "unicorn_block.h"

#define BLOCK_FACTORY_COUNT 31

//TODO: is this file really needed?
namespace u
{
	extern const Block* block_factory[BLOCK_FACTORY_COUNT];

	type::n* work_dummy(port** portlist);
	NodeTuneResult tune_dummy(Node* node, NodeTuneType tune_type);
	extern const char* string_dummy;

	extern const Block lib_const[];
	extern const size_t lib_const_count;

	extern const Block lib_standard[];
	extern const size_t lib_standard_count;

	extern const Block lib_linecode[];
	extern const size_t lib_linecode_count;

}
#endif
