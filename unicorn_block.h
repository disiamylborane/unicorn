
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
	/*
	class Block
	{
	public:
		port **portlist;
		Block* next;
		int16_t xpos, ypos;
		uint8_t inputs, outputs, ports, reserved;
		uint32_t factory_index;

		virtual void work() { return; };
		virtual void tune() { return; };
		virtual const char* ports_cfg() { return ""; };

		char get_port_symbol(int index);

		bool setup_ports();
		void destroy_ports();
	};*/
	
	struct Block
	{
		void (*work)(port** portlist);
		void (*tune)();
		const char* ports_cfg;
		const char* name;
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1
		const char* description;
#endif
	};
	
	class Node
	{
	public:
		port **portlist;
		Node* next;
		int16_t xpos, ypos;
		uint8_t inputs, outputs, ports, reserved;
		
		const Block *core;
		
		char get_port_symbol(int index);
		bool setup_ports();
		void destroy_ports();
	};

	void run_blocks(Node* start);
}

#endif
