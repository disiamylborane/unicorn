
#include <string.h>
#include "unicorn_graph.h"

namespace u {
	Graph::Graph(int reserve_nodes, int reserve_stencil, int reserve_const) {
		nodes = new uniseq(sizeof (Node*), reserve_nodes);
		stencil_buffer = new uniseq(sizeof(StencilBuffer), reserve_stencil);
	}

	Graph::~Graph() {
		Node** begin = (Node**)nodes->begin;
		for (Node **b = begin; b < &begin[nodes->size]; b++){
			destroy_ports(*b);
			delete (*b);
		}
		delete nodes;
		delete stencil_buffer;
	}

	void Graph::_add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin) {
		StencilBuffer el = { bout , bin , pout , pin };
		stencil_buffer->push_back(&el);
	}
	void Graph::_del_stencil(int index) {
		stencil_buffer->remove(index);
	}

	void Graph::add_std_node(int factory_index, int xpos, int ypos) {
		Node* nd = new_std_node(factory_index);
		nd->xpos = xpos;
		nd->ypos = ypos;
		nodes->push_back_pointer(nd);
	}

	void Graph::del_node(int index) {
		nodes->remove(index);

		for (unsigned int i = 0; i < stencil_buffer->size; i++) {
			StencilBuffer* stencil = (StencilBuffer*)stencil_buffer->at(i);
			if (stencil->in_node > index)
				stencil->in_node -= 1;
			if (stencil->out_node > index)
				stencil->out_node -= 1;

			if (stencil->out_node == index) {
				((Node**)nodes->begin)[stencil->in_node]->portlist[stencil->in_port] = 0;
			}
			if (stencil->in_node != index)
				continue;
			stencil_buffer->remove(i);
			i--;
		}
	}

	void Graph::move(int node, int deltax, int deltay) {
		((Node**)nodes->begin)[node]->xpos += deltax;
		((Node**)nodes->begin)[node]->ypos += deltay;
	}

	Graph::_port_type Graph::_get_port_type(int node, int port) {
		Node* _working = ((Node**)nodes->begin)[node];
		return get_port_type(_working, port);
	}

	void Graph::link(int node, int port, int to) {
		((Node**)nodes->begin)[node]->portlist[port] = ((Node**)nodes->begin)[to];
	}
	void Graph::unlink(int node, int port) {
		((Node**)nodes->begin)[node]->portlist[port] = nullptr;
	}

	bool Graph::connect(int node1, int port1, int node2, int port2){
		//The ports can be connected only <Free> to <Rigid>

		_port_type _t1 = _get_port_type(node1, port1);
		_port_type _t2 = _get_port_type(node2, port2);

		if ((_t1 == UNICORN_PORT_ERROR) || (_t1 == UNICORN_PORT_ERROR) || (_t1 == _t2))
			return false;

		int nodeI, nodeO, portI, portO;
		if (_t1 == UNICORN_PORT_RIGID) {
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
		((Node**)nodes->begin)[nodeI]->portlist[portI] = ((Node**)nodes->begin)[nodeO]->portlist[portO];
		
		return true;
	}

	void Graph::disconnect(int node, int port) {
		for (unsigned int i = 0; i < stencil_buffer->size;) {
			if (((((StencilBuffer*)stencil_buffer->begin)[node].in_node == node) && (((StencilBuffer*)stencil_buffer->begin)[i].in_port == port)) ||
				((((StencilBuffer*)stencil_buffer->begin)[i].out_node == node) && (((StencilBuffer*)stencil_buffer->begin)[i].out_port == port)))
			{
				//TODO: refactor this dinosaur
				((Node**)nodes->begin)[((StencilBuffer*)stencil_buffer->begin)[i].in_node]->portlist[((StencilBuffer*)stencil_buffer->begin)[i].in_port] = 0;

				_del_stencil(i);
			}
			else i++;
		}
	}

	void Graph::run(int index)
	{
		run_blocks(((Node**)nodes->begin)[index]);
	}
}
