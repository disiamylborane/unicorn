
#ifndef UNICORN_GRAPH_H_GUARD
#define UNICORN_GRAPH_H_GUARD

#include "unicorn_cfg.h"
#include "unicorn_block.h"

namespace u {

	class Mark;
	class Graph;

	class Mark {
		friend void pspec_ui(Graph *g);

	public:
		uniseq *name; //string

		uint16_t node;
		uint8_t port;

		Mark(int reserve = 5);
		Mark(const char* _name);
		~Mark();
	};


	//TODO: add clean method, replace platform_specific <clear> behaviour
	// note: now cleaning the graph is calling destructor + placement new
	class Graph {
		friend void pspec_ui(Graph *g);
	protected:
		struct Stencil {
			uint16_t out_node;
			uint16_t in_node;
			uint8_t out_port;
			uint8_t in_port;
		};

		uniseq *stencil_buffer;
	public:
		uniseq *node_buffer;
		uniseq *mark_buffer;

	protected:
		PortLocation _get_port_location(int node, int port);
		void _add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin);
		void _del_stencil(int index);
		int _port_marked(int node, int port);
		int _port_connected(int node, int port);
		//int _node_marked(int node);

		void clear_nodes();

	public:
		Graph(int reserve_nodes = 16, int reserve_stencil = 32, int reserve_mark = 4);
		~Graph();

		void clear();

		void add_node(const Block* block, int xpos, int ypos, Function* hierarchical_func = nullptr);
		void del_node(int index);
		Node* get_node(int index);

		void move(int node, int deltax, int deltay);

		bool link(int node, int port, int to);
		void unlink(int node, int port);

		bool connect(int node1, int port1, int node2, int port2);
		void disconnect(int node, int port);

		bool mark(uint16_t node, uint8_t port, const char* name);
		void unmark(uint16_t markindex);

		void tune_node(int node);
		void ready();
		void run(int index);
	};

	class Function {
	protected:

		//The first member is always <parent> Function*
		uniseq ports_cfg;

	public:
		Graph routine;
		Block represent;
		Node* start_node;

	public:
		Function(const char* name, const char* desc = nullptr);
		~Function();

	protected:
		void update_block_hard(int markcount);

	public:
		//Return whether ports_cfg is updated and the parents must delete all node_buffer of this block
		bool update_block();
	};
	void pspec_ui(Graph *g);
}

#endif
