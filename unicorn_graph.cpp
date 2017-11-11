
#include <new>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "unicorn_graph.h"
#include "unicorn_library.h"

#define NODE ((Node**)node_buffer->begin)
#define PORT(node,port) (NODE[node]->portlist[port])
#define STENCIL ((Stencil*)stencil_buffer->begin)
#define MARK ((Mark*)mark_buffer->begin)

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
		node_buffer = new uniseq(sizeof (Node*), reserve_nodes);
		stencil_buffer = new uniseq(sizeof(Stencil), reserve_stencil);
		mark_buffer = new uniseq(sizeof(Mark), reserve_mark);
	}

	Graph::~Graph() {
		clear_nodes();
		delete node_buffer;
		delete stencil_buffer;
		delete mark_buffer;
	}
	void Graph::clear_nodes() {
		for (Node **b = NODE; b < NODE + node_buffer->size; b++) {
			destroy_ports(*b);
			delete (*b);
		}
	}
	void Graph::clear() {
		clear_nodes();

		node_buffer->clear();
		stencil_buffer->clear();
		mark_buffer->clear();
	}

	void Graph::_add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin) {
		Stencil el = { bout , bin , pout , pin };
		stencil_buffer->push_back(&el);
	}
	void Graph::_del_stencil(int index) {
		stencil_buffer->remove(index);
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
		Stencil *s = STENCIL;
		for (int i = 0; i < node_buffer->size; i++, s++) {
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
		node_buffer->push_back_pointer(nd);

		PortDefinition pd = node_port_get_definition(nd->core, 0);
		if (node_port_get_datatype(pd) == UTYPE_g[0])
			nd->portlist[0] = (void*)hierarchical_func;
	}

	void Graph::del_node(int index) {
		node_buffer->remove(index);

		for (unsigned int i = 0; i < stencil_buffer->size; i++) {
			Stencil* stencil = STENCIL + i;
			if (stencil->in_node > index)
				stencil->in_node -= 1;
			if (stencil->out_node > index)
				stencil->out_node -= 1;

			if (stencil->out_node == index) {
				PORT(stencil->in_node, stencil->in_port) = 0;
			}
			if (stencil->in_node == index) {
				stencil_buffer->remove(i);
				i--;
			}
		}

		for (unsigned int i = 0; i < mark_buffer->size; i++) {
			Mark* m = (Mark*)mark_buffer->at(i);
			if (m->node > index)
				m->node--;
			else if (m->node == index) {
				mark_buffer->remove(i);
				i--;
			}
		}
	}

	Node* Graph::get_node(int index) {
		if (index > node_buffer->size)
			return nullptr;
		return NODE[index];
	}

	void Graph::move(int node, int deltax, int deltay) {
		NODE[node]->xpos += deltax;
		NODE[node]->ypos += deltay;
	}

	PortLocation Graph::_get_port_location(int node, int port) {
		Node* _working = NODE[node];
		PortDefinition pd = node_port_get_definition(_working->core, port);
		return node_port_get_location(pd);
	}

	bool Graph::link(int node, int port, int to) {
		if (_port_marked(node, port) != -1)
			return false;
		PORT(node,port) = NODE[to];
		_add_stencil(to, 255,node,port);
		return true;
	}
	void Graph::unlink(int node, int port) {
		PORT(node, port) = nullptr;
		for (unsigned int i = 0; i < stencil_buffer->size;) {
			auto cstencil = &STENCIL[i];

			if ((cstencil->in_node == node) && (cstencil->in_port == port))
				_del_stencil(i);
			else i++;
		}
	}

	bool Graph::connect(int node1, int port1, int node2, int port2){
		if (_port_marked(node1, port1) != -1)
			return false;
		if (_port_marked(node2, port2) != -1)
			return false;
		//The ports can be connected only <External> to <Internal>

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
		PORT(nodeI, portI) = PORT(nodeO, portO);
		
		return true;
	}

	bool Graph::mark(uint16_t node, uint8_t port, const char* name) {
		if (_port_marked(node, port) != -1)
			return false;
		if (_port_connected(node, port) != -1)
			return false;

		Mark _temp(0);
		mark_buffer->push_back(&_temp);
		Mark* work = MARK + mark_buffer->size - 1;
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
			auto cstencil = &STENCIL[i];

			if (((cstencil->in_node == node) && (cstencil->in_port == port)) ||
				((cstencil->out_node == node) && (cstencil->out_port == port)))
			{
				PORT(cstencil->in_node, cstencil->in_port) = 0;

				_del_stencil(i);
			}
			else i++;
		}
	}

	void Graph::tune_node(int node)
	{
		if (node >= node_buffer->size)
			return;
		Node *n = NODE[node];
		n->core->tune(n, ntt_manual);
	}
	void Graph::ready()
	{
		for (Node **b = NODE; b < NODE + node_buffer->size; b++) {
			(*b)->core->tune(*b, ntt_start);
		}
	}
	void Graph::run(int index)
	{
		run_blocks(NODE[index]);
	}
	void fd_write_symbol(int fd, char sym)
	{
		char fd_write_symbol_char = sym;
		write(fd, &fd_write_symbol_char, 1);
	}
	int fd_read_number(int fd)
	{
	    char buf;
	    int ret = 0;
	    buf=read(fd, &buf, 1);
	    while (true) {
	        buf=read(fd, &buf, 1);
	        if((buf< '0') || (buf > '9'))
	            return ret;
	        ret *= 10;
	        ret += (int)(buf - '0');
	    };
	}
	static const Block* search_std_lib(char* s)
	{
		for (int lib = 0; lib < libraries_count; lib++) {
			for (int bl = 0; bl < libraries[lib].count; bl++) {
				if (strcmp(s, libraries[lib].blocks[bl].name) == 0) {
					return &libraries[lib].blocks[bl];
				}
			}
		}
		return NULL;
	}
	void Graph::save_file(int fd)
	{
		int nodescnt = node_buffer->size;
		write(fd, &nodescnt, sizeof(nodescnt));
		for(Node **b = NODE; b < NODE + node_buffer->size; b++){
			const char* nodename = (*b)->core->name;
			write(fd, nodename, strlen(nodename));
			fd_write_symbol(fd, '\0');
			write(fd, &(*b)->xpos, sizeof((*b)->xpos));
			write(fd, &(*b)->ypos, sizeof((*b)->ypos));
			for (int port = 0; port < (*b)->ports; port++) {
				PortDefinition def = node_port_get_definition((*b)->core, port);
				if (node_port_get_location(def) == pl_internal) {
					char dtype = node_port_get_datatype(def);
					if(is_array_type(dtype)){
						dtype = get_arr_type(dtype);
						int size = ((uniseq*)((*b)->portlist[port]))->size;
						write(fd, &size, sizeof(size));
						write(fd, ((uniseq*)((*b)->portlist[port]))->begin, get_type_size(dtype)*size);
					} else {
						write(fd, (*b)->portlist[port], get_type_size(dtype));
					}
				}
			}
		}

		int stencilcnt = stencil_buffer->size;
		write(fd, &stencilcnt, sizeof(stencilcnt));
		write(fd, STENCIL, sizeof(Stencil)*stencilcnt);
		/*
		write(fd, &mark_buffer->size, sizeof(mark_buffer->size));
		for (unsigned int i = 0; i < mark_buffer->size; i++) {
			Mark* m = (Mark*)mark_buffer->at(i);
			write(fd, m->name->begin, strlen((char*)m->name->begin));
			fd_write_symbol(fd, '@');
			fd_write_number(fd, m->node);
			fd_write_symbol(fd, ':');
			fd_write_number(fd, m->port);
		}*/
	}

	bool Graph::load_file(int fd)
	{
		char namebuffer[32];
		
		clear();

		int nodescnt;
		read(fd, &nodescnt, sizeof(nodescnt));
		node_buffer->reserve(nodescnt);
		for (int nd = 0; nd < nodescnt; nd++) {
			int i = 0;
			while (true) {
				read(fd, &namebuffer[i], 1);
				if (namebuffer[i] == '\0')
					break;
				i++;
				if (i == 32)
					goto error;
			}
			const Block* bl = search_std_lib(namebuffer);
			if (!bl)
				goto error;

			int16_t xpos,ypos;
			read(fd, &xpos, sizeof(xpos));
			read(fd, &ypos, sizeof(ypos));

			add_node(bl, xpos, ypos);

			Node* bnd = *((Node**)node_buffer->back());
			for (int port = 0; port < bnd->ports; port++) {
				PortDefinition def = node_port_get_definition(bnd->core, port);
				if (node_port_get_location(def) == pl_internal) {
					char dtype = node_port_get_datatype(def);
					if (is_array_type(dtype)) {
						dtype = get_arr_type(dtype);
						int size;
						read(fd, &size, sizeof(size));
						((uniseq*)bnd->portlist[port])->resize(size);
						read(fd, ((uniseq*)(bnd->portlist[port]))->begin, get_type_size(dtype)*size);
					}
					else {
						read(fd, bnd->portlist[port], get_type_size(dtype));
					}
				}
			}
		}

		int stencilcnt;
		read(fd, &stencilcnt, sizeof(stencilcnt));
		stencil_buffer->resize(stencilcnt);
		read(fd, STENCIL, sizeof(Stencil)*stencilcnt);
		for (int i = 0; i < stencilcnt; i++) {
			Stencil *st = STENCIL + i;
			if (st->out_port == 255)
				NODE[st->in_node]->portlist[st->in_port] = NODE[st->out_node];
			else
				NODE[st->in_node]->portlist[st->in_port] = NODE[st->out_node]->portlist[st->out_port];
		}

		return true;

	error:
		clear();
		return false;
	}

	extern NodeTuneResult tune_dummy(Node* node, NodeTuneType tune_type);
	static type::n* work_FunctionBlock(port** portlist)
	{
		type::g* fn = ((type::g*)portlist[0]);
		uniseq *nodes = fn->routine.node_buffer;
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
		represent.tune = tune_dummy;
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
			PortDefinition defgr = node_port_get_definition(((Node**)(routine.node_buffer->begin))[mark->node]->core, mark->port);

			ports_cfg.push_back_8(defgr[0]);
			ports_cfg.push_back_8(defgr[1]);
			ports_cfg.append(mark->name, 0);
			ports_cfg.size--;//Pop back <zero> value
		}
		ports_cfg.push_back_8(0);
		represent.ports_cfg = (char*)ports_cfg.begin;
	}

	bool Function::update_block() {
		bool update_soft = false;
		int markcount = routine.mark_buffer->size;
		if (block_port_get_count(represent.ports_cfg) == markcount+1) {
			for (int i = 0; i < markcount; i++) {
				PortDefinition defbl = node_port_get_definition(&represent, i+1);
				Mark* mark = &(((Mark*)routine.mark_buffer->begin)[i]);
				PortDefinition defgr = node_port_get_definition(((Node**)(routine.node_buffer->begin))[mark->node]->core, mark->port);

				//TODO: unhardcode and rename
				if ((defbl[0] != defgr[0]) && (defbl[1] != defgr[1])) {
					update_block_hard(markcount);
					return true;
				}

				if (!defs_equal(defbl+2, defgr+2)) {
					update_soft = true;
				}
			}
			if(update_soft)
				update_block_hard(markcount);
			return false;
		}
		update_block_hard(markcount);
		return true;
	}
}
