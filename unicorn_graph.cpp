
#include <string.h>
#include "unicorn_graph.h"

namespace u {

	Graph::Graph(int reserve_nodes, int reserve_stencil, int reserve_const) {
		nodes = new vector<Node*>;
		nodes->reserve(reserve_nodes);
		stencil_buffer = new vector<StencilBuffer>;
		stencil_buffer->reserve(reserve_stencil);
		const_buffer = new vector<void*>;
		const_buffer->reserve(reserve_const);
	}

	Graph::~Graph() {
		for (Node *b : *nodes) {
			destroy_ports(b);
			delete b;
		}
		//TODO: delete const_buffer array elements
		//....separate const_buffer and const_array_buffer
		delete nodes;
		delete stencil_buffer;
		delete const_buffer;
	}

	void Graph::_add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin) {
		stencil_buffer->push_back({ bout , bin , pout , pin });
	}
	void Graph::_del_stencil(int index) {
		stencil_buffer->erase(stencil_buffer->begin() + index);
	}
	void *Graph::_add_const(size_t size) {
		void *ret = U_MALLOC(size);
		const_buffer->push_back(ret);
		return ret;
	}
	uniseq *Graph::_add_const_array(uint8_t size, uniseq* copyfrom) {
		uniseq *arr = new uniseq(size, copyfrom->size);
		arr->append(copyfrom,0);
		const_buffer->push_back(arr);
		return arr;
	}
	void Graph::_del_const(int index) {
		U_FREE((*const_buffer)[index]);
		const_buffer->erase(const_buffer->begin() + index);
	}
	void Graph::_del_const_array(int index) {
		delete (uniseq*)(*const_buffer)[index];
		const_buffer->erase(const_buffer->begin() + index);
	}

	void Graph::add_std_node(int factory_index, int xpos, int ypos) {
		Node* nd = new_std_node(factory_index);
		nd->xpos = xpos;
		nd->ypos = ypos;
		nodes->push_back(nd);
	}

	void Graph::del_node(int index) {
		nodes->erase(nodes->begin() + index);

		for (unsigned int i = 0; i < stencil_buffer->size(); i++) {
			StencilBuffer &stencil = stencil_buffer->at(i);
			if (stencil.in_node > index)
				stencil.in_node -= 1;
			if (stencil.out_node > index)
				stencil.out_node -= 1;

			if (stencil.out_node == index) {
				(*nodes)[stencil.in_node]->portlist[stencil.in_port] = 0;
			}
			if (stencil.in_node != index)
				continue;
			stencil_buffer->erase(stencil_buffer->begin() + i);
			i--;
		}
	}

	void Graph::move(int node, int deltax, int deltay) {
		(*nodes)[node]->xpos += deltax;
		(*nodes)[node]->ypos += deltay;
	}

	Graph::_port_type Graph::_get_port_type(int node, int port) {
		Node* _working = (*nodes)[node];
		int ins = _working->inputs;
		if (ins > port)
			return _c_in;
		int ports = _working->ports;
		if (ports > port)
			return _c_out;
		return _c_err;
	}

	void Graph::link(int node, int port, int to) {
		(*nodes)[node]->portlist[port] = (*nodes)[to];
	}
	void Graph::unlink(int node, int port) {
		(*nodes)[node]->portlist[port] = nullptr;
	}

	bool Graph::connect(int node1, int port1, int node2, int port2){
		_port_type _t1 = _get_port_type(node1, port1);
		_port_type _t2 = _get_port_type(node2, port2);

		if ((_t1 == _c_err) || (_t1 == _c_err) || (_t1 == _t2))
			return false;

		int nodeI, nodeO, portI, portO;
		if (_t1 == _c_out) {
			nodeI = node2;
			nodeO = node1;
			portI = port2;
			portO = port1;
		}
		else {
			nodeI = node1;
			nodeO = node2;
			portI = port1;
			portO = port2;
		}
		_add_stencil(nodeO, portO, nodeI, portI);
		(*nodes)[nodeI]->portlist[portI] = (*nodes)[nodeO]->portlist[portO];
		
		return true;
	}

	void Graph::disconnect(int node, int port) {
		if (_get_port_type(node, port) == _c_in) {
			for (size_t i = 0; i < const_buffer->size(); i++) {
				if ((*nodes)[node]->portlist[port] == (*const_buffer)[i])
				{
					(*nodes)[node]->portlist[port] = 0;
					char tsym = get_port_symbol((*nodes)[node],port);
					if (is_array_type(tsym))
						_del_const_array(i);
					else
						_del_const(i);
					return;
				}
			}
		}

		for (unsigned int i = 0; i < stencil_buffer->size();) {
			if ((((*stencil_buffer)[i].in_node == node) && ((*stencil_buffer)[i].in_port == port)) ||
				(((*stencil_buffer)[i].out_node == node) && ((*stencil_buffer)[i].out_port == port)))
			{
				(*nodes)[(*stencil_buffer)[i].in_node]->portlist[(*stencil_buffer)[i].in_port] = 0;

				_del_stencil(i);
			}
			else i++;
		}
	}

	bool Graph::connect_const(int node, int port, void* data) {
		if ((*nodes)[node]->inputs <= port)
			return false;

		char sym = get_port_symbol((*nodes)[node],port);
		if (is_array_type(sym)){
			uniseq* fill = (uniseq*)data;
			uniseq* newconst = _add_const_array(get_arr_size(sym), fill);
			(*nodes)[node]->portlist[port] = newconst;
		}
		else {
			void *newconst;
			size_t size;
			size = get_type_size(sym);
			newconst = _add_const(size);
			(*nodes)[node]->portlist[port] = newconst;
			memcpy(newconst, data, size);
		}

		return true;
	}
	void Graph::run(int index)
	{
		run_blocks((*nodes)[index]);
	}
}
