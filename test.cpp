
#include <new>

#include "unicorn_library.h"
#include "unicorn_graph.h"

extern void u::pspec_ui(u::Graph*);
extern u::uniseq global_functions;

static const u::Block* libs[] = { u::lib_const.blocks, u::lib_standard.blocks, u::lib_linecode.blocks };

int main(){
	new (&global_functions) u::uniseq(sizeof(u::Function*), 4);

	u::Graph* g = new u::Graph();
	
	g->add_node(&libs[0][4], 0, 0); // 0: int
	g->add_node(&libs[0][4], 0, 0); // 1: int
	*(int*)((u::Node**)g->node_buffer->begin)[0]->portlist[0]=5;
	*(int*)((u::Node**)g->node_buffer->begin)[1]->portlist[0]=6;
	
	g->add_node(&libs[1][0], 0, 0); // 2: +
	g->connect(2,1,0,0);
	g->connect(2,2,1,0);
	
	g->add_node(&libs[2][0], 0, 0); // 3: pcm
	g->connect(3,1,2,3); //code is sum result
	g->add_node(&libs[0][2], 0, 0); // 4: byte
	*(char*)((u::Node**)g->node_buffer->begin)[4]->portlist[0]=13;
	g->connect(3,2,4,0); //bytecnt
	g->add_node(&libs[0][14], 0, 0); // 5: int array
	{
	    u::uniseq* seq = ((u::uniseq*)((u::Node**)g->node_buffer->begin)[5]->portlist[0]);
        seq->clear();
	    seq->push_back_32(1200);
	    seq->push_back_32(600);
	}
	g->connect(3,3,5,0); //array 1
	g->add_node(&libs[0][14], 0, 0); // 5: int array
	{
	    u::uniseq* seq = ((u::uniseq*)((u::Node**)g->node_buffer->begin)[6]->portlist[0]);
        seq->clear();
	    seq->push_back_32(600);
	    seq->push_back_32(600);
	}
	g->connect(3,4,6,0); //array 0
	
	g->link(2, 0, 3);
	
	u::pspec_ui(g);

	global_functions.~uniseq();
	delete g;
}


