
#ifndef UNICORN_GRAPH_H_GUARD
#define UNICORN_GRAPH_H_GUARD

#include "unicorn_library.h"

using namespace std;

namespace u {
	
	//TODO: add clean method, replace platform_specific <clear> behaviour
	// note: now cleaning the graph is calling destructor + placement new
	class Graph {
		friend bool pspec_ui(Graph *g);
	protected:
		struct StencilBuffer {
			uint16_t out_node;
			uint16_t in_node;
			uint8_t out_port;
			uint8_t in_port;
		};
		uniseq *nodes;
		uniseq *stencil_buffer;

		PortLocation _get_port_location(int node, int port);
		void _add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin);
		void _del_stencil(int index);

	public:
		Graph(int reserve_nodes = 16, int reserve_stencil = 32, int reserve_const = 16);
		~Graph();

		void add_node(const Block* block, int xpos, int ypos);
		void del_node(int index);

		void move(int node, int deltax, int deltay);

		void link(int node, int port, int to);
		void unlink(int node, int port);

		bool connect(int node1, int port1, int node2, int port2);
		void disconnect(int node, int port);

		void tune();
		void run(int index);
	};

	bool pspec_ui(Graph *g);
}

#endif
