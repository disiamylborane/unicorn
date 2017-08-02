
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
	static uint8_t _psym_size(char psym)
	{
		return get_type_size(psym);
	}

	bool setup_ports(Node* node)
	{
		node->inputs = 0;
		node->outputs = 0;
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
			uint8_t _size;
			if (!is_array_type(psym)){
				_size = _psym_size(psym);
				if (!_size) {
					return false;
				}
				outs_length = _align(outs_length, _size);
				outs_length += _size;
			}
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
				uniseq* _new_arr = new uniseq(get_arr_size(psym), UNICORN_CFG_uniseq_BLOCK_RESERVE);
				if(!_new_arr)
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

	void destroy_ports(Node* node)
	{
		port** outlist = &node->portlist[node->inputs];
		for (int i = 0; i < node->outputs; i++) {
			char psym = get_port_symbol(node, i + node->inputs);
			if (is_array_type(psym)) {
				delete (uniseq*)outlist[i];
			}
		}
		U_FREE(node->portlist);
	}
	
	static const char* _search_symbol(Node* node, int port)
	{
		for (const char*c = node->core->ports_cfg; *c; c++) 
		{
			if (*c == UNICORN_CFG_BLOCK_TYPEMARK) 
			{
				if (port == 0){
					return c+1;
				}
				else port--;
			}
		}
		return nullptr;
	}
	
	char get_port_symbol(Node* node, int port)
	{
		const char* s = _search_symbol(node, port);
		if(!s)
			return '\0';
		return *s;
	}
	
	const char* get_port_name_pointer(Node* node, int port)
	{
		const char* s = _search_symbol(node, port);
		if(!s)
			return nullptr;
		return &s[1];
	}
	
	Node* new_node(const Block* bl)
	{
		Node* ret = (Node*)U_MALLOC(sizeof(Node));
		if(!ret)
			return nullptr;
		ret->core = bl;
		if (!setup_ports(ret)) {
			U_FREE(ret);
			return nullptr;
		}
		return ret;
	}
	
	void free_node(Node* nd){
		destroy_ports(nd);
		free(nd);
	}

	void run_blocks(Node* start) {
		while (start) {
			start = (Node*)start->core->work(start->portlist);
		}
	}
}

