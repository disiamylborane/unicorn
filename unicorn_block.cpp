
#include <string.h>
#include "unicorn_block.h"

namespace u
{
	static size_t _align(size_t outs_length, uint8_t size) {
		uint8_t __align = outs_length % size;
		if (__align)
			outs_length += size - __align;
		return outs_length;
	}
	static uint8_t _psym_size(char psym) {
		return get_type_size(psym);
	}

	bool setup_ports(Node* node) {
		uint8_t *_write = &node->inputs;
		for (const char*c = node->core->ports_cfg; *c; c++) {
			if (*c == UNICORN_CFG_BLOCK_DECOUPLER) {
				_write = &node->outputs;
			}
			if (*c == UNICORN_CFG_BLOCK_TYPEMARK)
				(*_write)++;
		}
		node->ports = node->inputs + node->outputs;

		size_t outs_length = 0;
		for (int i = 0; i < node->outputs; i++) {
			char psym = get_port_symbol(node, i + node->inputs);
			if (is_array_type(psym))
				continue;
			uint8_t _size = _psym_size(psym);
			if (!_size) {
				return false;
			}
			outs_length = _align(outs_length, _size);
			outs_length += _size;
		}

		size_t whole_ports_size = sizeof(void*)*(node->ports)+outs_length;
		node->portlist = (port**)U_MALLOC(whole_ports_size);
		if (!node->portlist) {
			return false;
		}
		memset(node->portlist, 0, whole_ports_size);
		port** outlist = &node->portlist[node->inputs];
		void* curr_port_address = &node->portlist[node->ports];
		for (int i = 0; i < node->outputs; i++) {
			char psym = get_port_symbol(node, i + node->inputs);
			if (is_array_type(psym)) {
				void* _new_arr = new_array_type(psym);
				if (!_new_arr)
					return false;
				outlist[i] = _new_arr;
			}
			else {
				uint8_t _size = _psym_size(psym);
				curr_port_address = (void*)_align((size_t)curr_port_address, _size);
				outlist[i] = curr_port_address;
				curr_port_address = (void*)((size_t)curr_port_address + _size);
			}
		}

		return true;
	}

	void destroy_ports(Node* node) {
		port** outlist = &node->portlist[node->inputs];
		for (int i = 0; i < node->outputs; i++) {
			char psym = get_port_symbol(node, i + node->inputs);
			if (is_array_type(psym)) {
				if(outlist[i])
					delete_array_type(psym, outlist[i]);
			}
		}
		U_FREE(node->portlist);
	}

	char get_port_symbol(Node* node, int index) {
		for (const char*c = node->core->ports_cfg; *c; c++) 
		{
			if (*c == UNICORN_CFG_BLOCK_TYPEMARK) 
			{
				if (index == 0){
					return c[1];
				}
				else index--;
			}
		}
		return '\0';
	}

	void run_blocks(Node* start) {
		while (start) {
			start = (Node*)start->core->work(start->portlist);
		}
	}
}

