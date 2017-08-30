
#ifndef UNICORN_BLOCK_H_GUARD
#define UNICORN_BLOCK_H_GUARD

#include "unicorn_types.h"
#include "unicorn_cfg.h"
#include "unicorn_macro.h"

namespace u
{	
	//ntt_manual: manually called in user interface
	//ntt_loaded: the block is now loaded from file
	//ntt_saving: the block wil now be saved to file
	//btt_begin: called by <Graph> before the work starts
	typedef enum NodeTuneType {ntt_manual, ntt_loaded, ntt_saving, ntt_start, ntt_stop} NodeTuneType;

	typedef enum NodeTuneResult { ntr_ok, ntr_portlist_changed, ntr_memory_error} NodeTuneResult;
	
	struct Block
	{
		type::n* (*work)(port** portlist);
		NodeTuneResult (*tune)(Node* node, NodeTuneType tune_type);
		const char* ports_cfg;
		const char* name;
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1
		const char* description;
#endif
	};
	
	struct Node
	{
		port **portlist;
		void* internal;
		int16_t xpos, ypos;
		uint8_t frees, ports, reserved0, reserved1;
		
		const Block *core;
	};
	
	bool setup_ports(Node* node);
	void destroy_ports(Node* node);

	typedef const char* PortDefinition;
	typedef enum PortLocation { pl_external = 0, pl_internal = 1, pl_error = 0x7F} PortLocation;
	typedef enum PortDirection { pd_input = false, pd_output = true } PortDirection;

	int block_port_get_count(const char *cfg);
	bool defs_equal(PortDefinition d1, PortDefinition d2);

	PortDefinition node_port_get_definition(const Block* bl, int port);

	PortLocation node_port_get_location(PortDefinition def);
	PortDirection node_port_get_direction(PortDefinition def);
	char node_port_get_datatype(PortDefinition def);
	const char* node_port_get_name(PortDefinition def);

	Node* new_node(const Block* bl);
	void free_node(Node* nd);
	void run_blocks(Node* start);
}

#endif
