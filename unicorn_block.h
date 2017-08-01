
#ifndef UNICORN_BLOCK_H_GUARD
#define UNICORN_BLOCK_H_GUARD

#include "unicorn_types.h"
#include "unicorn_cfg.h"

#ifndef UNICORN_CFG_BLOCK_DESCRIPTIONS
#error "UNICORN_CFG_BLOCK_DESCRIPTIONS not defined"
#endif
#if (UNICORN_CFG_BLOCK_DESCRIPTIONS != 1) && (UNICORN_CFG_BLOCK_DESCRIPTIONS != 0)
#error "Wrong UNICORN_CFG_BLOCK_DESCRIPTIONS parameter"
#endif

namespace u
{
	struct Node;
	namespace type {
		typedef struct Node n;
	}
	
	struct Block
	{
		type::n* (*work)(port** portlist);
		void (*tune)();
		const char* ports_cfg;
		const char* name;
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1
		const char* description;
#endif
	};
	
	struct Node
	{
		port **portlist;
		int16_t xpos, ypos;
		uint8_t inputs, outputs, ports, reserved;
		
		const Block *core;
	};
	
	char get_port_symbol(Node* node, int index);
	bool setup_ports(Node* node);
	void destroy_ports(Node* node);

	void run_blocks(Node* start);
}

#endif
