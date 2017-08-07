
#ifndef UNICORN_LIBRARY_H_GUARD
#define UNICORN_LIBRARY_H_GUARD

#include "unicorn_cfg.h"
#include "unicorn_block.h"

//TODO: is this file really needed?
namespace u
{
	extern const Block* block_factory[];

	Node* new_std_node(int factory_index);
}
#endif
