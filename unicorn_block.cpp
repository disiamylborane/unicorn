
#include <string.h>
#include "unicorn_block.h"

namespace u
{
	static size_t _align(size_t outs_length, uint8_t size)
	{
		uint8_t __align = outs_length % size;
		if (__align)
			outs_length += size - __align;
		return outs_length;
	}

	bool setup_ports(Node* node)
	{
		//Calculate how much memory is needed for all the rigid ports
		//Also fill <ports> and <exts> fields of <node>
		uint8_t exts = 0;
		uint8_t ints = 0;
		size_t memsize = 0;
		for (const char *c = node->core->ports_cfg; *c; c++) {
			char curr = *c;
			if (curr == UNICORN_PORT_EXTERNAL) {
				exts++;
			}
			else if (curr == UNICORN_PORT_INTERNAL) {
				ints++;
				char ptype = c[1] & 0x7F;  // bit [7] render type , bits [6:0] data type
				if (!is_array_type(ptype)) {
					uint8_t _size = get_type_size(ptype);
					if (!_size)
						return false;
					memsize = _align(memsize, _size);
					memsize += _size;
				}
			}
		}
		node->frees = exts;
		uint8_t ports = exts + ints;
		node->ports = ports;
		size_t whole_ports_size = sizeof(void*)*(node->ports) + memsize;

		//Allocate memory for the ports
		port** portlist = (port**)U_PORTS_MALLOC(whole_ports_size);
		if (!portlist)
			return false;
		else 
			node->portlist = portlist;
		memset(portlist, 0, whole_ports_size);

		void* curr_port_address = &portlist[ports];

		//Use the <portlist> pointer to fill in internal ports
		for (const char *c = node->core->ports_cfg; *c; c++) {
			if (*c == UNICORN_PORT_INTERNAL) {
				char ptype = c[1] & 0x7F;
				if (is_array_type(ptype)) {
					uniseq* _new_arr = new uniseq(get_arr_size(ptype), UNICORN_CFG_uniseq_BLOCK_RESERVE);
					if (!_new_arr) {
						destroy_ports(node);
						return false;
					}
					*portlist = _new_arr;
				}
				else { //The validity of symbol has already been checked
					uint8_t _size = get_type_size(ptype);
					curr_port_address = (void*)_align((size_t)curr_port_address, _size);
					*portlist = curr_port_address;
					curr_port_address = (void*)((size_t)curr_port_address + _size);
				}
				portlist++;
			}
			else //if (*c == UNICORN_PORT_EXTERNAL)
				portlist++;
		}
		return true;
	}

	void destroy_ports(Node* node)
	{
		//If there are any rigid uniseqs, they must be deleted
		port** portlist = node->portlist;
		for (const char *c = node->core->ports_cfg; *c; c++){
			if (UNICORN_SYMBOL_IS_PORT(*c)) {
				if (*c == UNICORN_PORT_INTERNAL) {
					char ptype = c[1] & 0x7F;
					if (is_array_type(ptype)) {
						uniseq* freeable = (uniseq*)*portlist;
						if (freeable)
							delete freeable;
					}
				}
				portlist++;
			}
		}
		U_PORTS_FREE(node->portlist);
	}

	PortDefinition node_port_get_definition(Node* node, int port) {
		for (const char*c = node->core->ports_cfg; *c; c++)
		{
			if (UNICORN_SYMBOL_IS_PORT(*c))
			{
				if (port == 0) {
					return c;
				}
				else port--;
			}
		}
		return nullptr;
	}

	PortLocation node_port_get_location(PortDefinition def)
	{
		if (!def)
			return pl_error;
		if ((def[0]) & UNICORN_SYMBOL_INTERNAL_MASK)
			return pl_internal;
		else
			return pl_external;
	}
	PortDirection node_port_get_direction(PortDefinition def)
	{
		if ((def[1]) & UNICORN_SYMBOL_OUTPUT_MASK)
			return pd_output;
		else
			return pd_input;
	}

	char node_port_get_datatype(PortDefinition def)
	{
		return (def[1] & 0x7F);
	}

	const char* node_port_get_name(PortDefinition def)
	{
		return &def[2];
	}
	
	Node* new_node(const Block* bl)
	{
		Node* ret = (Node*)U_NODE_MALLOC(sizeof(Node));
		if(!ret)
			return nullptr;
		ret->core = bl;
		if (!setup_ports(ret)) {
			U_NODE_FREE(ret);
			return nullptr;
		}
		return ret;
	}
	
	void free_node(Node* nd){
		destroy_ports(nd);
		U_NODE_FREE(nd);
	}

	void run_blocks(Node* start) {
		while (start) {
			start = (Node*)start->core->work(start->portlist);
		}
	}
}

