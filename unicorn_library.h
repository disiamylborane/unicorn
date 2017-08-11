
#ifndef UNICORN_LIBRARY_H_GUARD
#define UNICORN_LIBRARY_H_GUARD

#include "unicorn_cfg.h"
#include "unicorn_block.h"

#define BLOCK_FACTORY_COUNT 22

//TODO: is this file really needed?
namespace u
{
	extern const Block* block_factory[BLOCK_FACTORY_COUNT];
}
#endif
