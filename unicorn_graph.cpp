
#include <new>
#include <string.h>
#include "unicorn_graph.h"

namespace u {
	Mark::Mark(int reserve) {
		name = new uniseq(sizeof(char), reserve);
	}
	Mark::Mark(const char* _name) {
		int reserve = strlen(_name) + 1;
		name = new uniseq(sizeof(char), reserve);
		name->resize(reserve);
		memcpy(name->begin, _name, reserve);
	}
	Mark::~Mark() {
		delete name;
	}

	Graph::Graph(int reserve_nodes, int reserve_stencil, int reserve_mark) {
		nodes = new uniseq(sizeof (Node*), reserve_nodes);
		stencil_buffer = new uniseq(sizeof(StencilBuffer), reserve_stencil);
		mark_buffer = new uniseq(sizeof(Mark), reserve_mark);
	}

	Graph::~Graph() {
		Node** begin = (Node**)nodes->begin;
		for (Node **b = begin; b < &begin[nodes->size]; b++){
			destroy_ports(*b);
			delete (*b);
		}
		delete nodes;
		delete stencil_buffer;
		delete mark_buffer;
	}

	void Graph::_add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin) {
		StencilBuffer el = { bout , bin , pout , pin };
		stencil_buffer->push_back(&el);
	}
	void Graph::_del_stencil(int index) {
		stencil_buffer->remove(index);
	}

	int Graph::_node_marked(int node) {
		Mark *m = (Mark*)mark_buffer->begin;
		for (int i = 0; i < mark_buffer->size; i++, m++) {
			if ((m->node == node))
				return i;
		}
		return -1;
	}

	int Graph::_port_marked(int node, int port) {
		Mark *m = (Mark*)mark_buffer->begin;
		for (int i = 0; i < mark_buffer->size; i++, m++) {
			if ((m->node == node) && (m->port == port))
				return i;
		}
		return -1;
	}

	int Graph::_port_connected(int node, int port) {
		StencilBuffer *s = (StencilBuffer*)stencil_buffer->begin;
		for (int i = 0; i < nodes->size; i++, s++) {
			if ((s->in_node == node) && (s->in_port == port))
				return i;
			if ((s->out_node == node) && (s->out_port == port))
				return i;
		}
		return -1;
	}

	void Graph::add_node(const Block* block, int xpos, int ypos, Function* hierarchical_func) {
		Node* nd = new_node(block);
		nd->xpos = xpos;
		nd->ypos = ypos;
		nodes->push_back_pointer(nd);

		PortDefinition pd = node_port_get_definition(nd->core, 0);
		if (node_port_get_datatype(pd) == UTYPE_g[0])
			nd->portlist[0] = (void*)hierarchical_func;
	}

	void Graph::del_node(int index) {
		while (true) {
			int markindex = _node_marked(index);
			if (markindex != -1)
				mark_buffer->remove(markindex);
			else
				break;
		}

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
		for (unsigned int i = 0; i < mark_buffer->size; i++) {
			Mark* m = (Mark*)mark_buffer->at(i);
			if (m->node > index)
				m->node--;
		}
	}

	Node* Graph::get_node(int index) {
		if (index > nodes->size)
			return nullptr;
		return ((Node**)nodes->begin)[index];
	}

	void Graph::move(int node, int deltax, int deltay) {
		((Node**)nodes->begin)[node]->xpos += deltax;
		((Node**)nodes->begin)[node]->ypos += deltay;
	}

	PortLocation Graph::_get_port_location(int node, int port) {
		Node* _working = ((Node**)nodes->begin)[node];
		PortDefinition pd = node_port_get_definition(_working->core, port);
		return node_port_get_location(pd);
	}

	bool Graph::link(int node, int port, int to) {
		if (_port_marked(node, port) != -1)
			return false;
		((Node**)nodes->begin)[node]->portlist[port] = ((Node**)nodes->begin)[to];
	}
	void Graph::unlink(int node, int port) {
		((Node**)nodes->begin)[node]->portlist[port] = nullptr;
	}

	bool Graph::connect(int node1, int port1, int node2, int port2){
		if (_port_marked(node1, port1) != -1)
			return false;
		if (_port_marked(node2, port2) != -1)
			return false;
		//The ports can be connected only <Free> to <Rigid>

		PortLocation _t1 = _get_port_location(node1, port1);
		PortLocation _t2 = _get_port_location(node2, port2);

		if ((_t1 == pl_error) || (_t1 == pl_error) || (_t1 == _t2))
			return false;

		int nodeI, nodeO, portI, portO;
		if (_t1 == pl_internal) {
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

	bool Graph::mark(uint16_t node, uint8_t port, const char* name) {
		if (_port_marked(node, port) != -1)
			return false;
		if (_port_connected(node, port) != -1)
			return false;

		Mark _temp(0);
		mark_buffer->push_back(&_temp);
		Mark* work = ((Mark*)mark_buffer->begin) + mark_buffer->size - 1;
		new (work) Mark(name);
		work->node = node;
		work->port = port;
		return true;
	}
	void Graph::unmark(uint16_t markindex) {
		Mark *m = (Mark*)mark_buffer->at(markindex);
		m->~Mark();
		mark_buffer->remove(markindex);
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

	void Graph::tune()
	{
		Node** begin = (Node**)nodes->begin;
		for (Node **b = begin; b < &begin[nodes->size]; b++) {
			(*b)->core->tune((*b)->portlist, btt_start);
		}
	}
	void Graph::run(int index)
	{
		run_blocks(((Node**)nodes->begin)[index]);
	}


	extern const char* tune_Dummy(port** portlist, BlockTuneType tune_type);
	static type::n* work_FunctionBlock(port** portlist)
	{
		type::g* fn = ((type::g*)portlist[0]);
		uniseq *nodes = fn->routine.nodes;
		uniseq *marks = fn->routine.mark_buffer;
		for (int i = 0; i < marks->size; i++)
		{
			Mark* cmark = ((Mark*)marks->begin) + i;
			Node* curr = ((Node**)nodes->begin)[cmark->node];
			curr->portlist[cmark->port] = portlist[i + 1];
		}

		return fn->start_node;
	}

	Function::Function(const char* name, const char* desc) {
		new (&ports_cfg) uniseq(sizeof(char), 16);

		ports_cfg.push_back_8(0);

		char* rname = new char[strlen(name)+1];
		strcpy(rname, name);
		represent.name = rname;
#if UNICORN_CFG_BLOCK_DESCRIPTIONS==1
		if (desc) {
			char* rdesc = new char[strlen(desc)+1];
			strcpy(rdesc, desc);
			represent.description = rdesc;
		}
		else represent.description = "";
#endif
		represent.ports_cfg = (char*)ports_cfg.begin;
		represent.work = work_FunctionBlock;
		represent.tune = tune_Dummy;
	}
	Function::~Function() {
		ports_cfg.~uniseq();
	}

	void Function::update_block_hard(int markcount) {
		ports_cfg.clear();

		ports_cfg.push_back_8(UNICORN_PORT_EXTERNAL);
		ports_cfg.push_back_8(UTYPE_DIR_g[0]);

		for (int m = 0; m < markcount; m++) {
			Mark* mark = &(((Mark*)routine.mark_buffer->begin)[m]);
			PortDefinition defgr = node_port_get_definition(((Node**)(routine.nodes->begin))[mark->node]->core, mark->port);

			ports_cfg.push_back_8(defgr[0]);
			ports_cfg.push_back_8(defgr[1]);
			ports_cfg.append(mark->name, 0);
			ports_cfg.size--;//Pop back <zero> value
		}
		ports_cfg.push_back_8(0);
		represent.ports_cfg = (char*)ports_cfg.begin;
	}

	bool Function::update_block() {
		int markcount = routine.mark_buffer->size;
		if (block_port_get_count(represent.ports_cfg) == markcount) {
			for (int i = 0; i < markcount; i++) {
				PortDefinition defbl = node_port_get_definition(&represent, i);
				Mark* mark = &(((Mark*)routine.mark_buffer->begin)[i]);
				PortDefinition defgr = node_port_get_definition(((Node**)(routine.nodes->begin))[mark->node]->core, mark->port);

				if (!defs_equal(defbl, defgr)) {
					update_block_hard(markcount);
					return true;
				}
			}
			return false;
		}
		update_block_hard(markcount);
		return true;
	}
}
