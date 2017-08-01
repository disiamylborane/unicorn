
#ifndef UNICORN_GRAPH_H_GUARD
#define UNICORN_GRAPH_H_GUARD

#include <vector>
#include "unicorn_library.h"

using namespace std;

namespace u {
	
	class Graph {
		friend bool pspec_ui(Graph *g);
	public:
		struct StencilBuffer {
			uint16_t out_node;
			uint16_t in_node;
			uint8_t out_port;
			uint8_t in_port;
		};
	protected:
		vector<Node*> *nodes;
		vector<StencilBuffer> *stencil_buffer;
		vector<void*> *const_buffer;

	protected:
		enum _port_type { _c_in, _c_out, _c_err };
		_port_type _get_port_type(int node, int port);
		void _add_stencil(uint16_t bout, uint8_t pout, uint16_t bin, uint8_t pin);
		void _del_stencil(int index);
		void _del_const(int index);
		void _del_const_array(char sym, int index);
		void *_add_const(size_t size);
		void *_add_const_array(char sym);

	public:
		Graph(int reserve_nodes = 16, int reserve_stencil = 32, int reserve_const = 16);
		~Graph();

		void add_std_node(int factory_index, int xpos, int ypos);
		void del_node(int index);

		void move(int node, int deltax, int deltay);

		void link(int bfrom, int bto);
		void unlink(int bfrom);

		bool connect(int node1, int port1, int node2, int port2);
		bool connect_const(int node, int port, void* data);
		void disconnect(int node, int port);
	};

	bool pspec_ui(Graph *g);
}

#endif
