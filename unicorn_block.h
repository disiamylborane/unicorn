
#ifndef UNICORN_BLOCK_H_GUARD
#define UNICORN_BLOCK_H_GUARD

#include "unicorn_types.h"

namespace u
{
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
	};

	void run_blocks(Block* start);
}

#endif
