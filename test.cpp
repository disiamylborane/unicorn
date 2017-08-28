

#include "unicorn_graph.h"
#include <iostream>

extern void u::pspec_ui(u::Graph*);

int main(){
	//u::Function *f = new u::Function("SuperFunction", "Super Function is the function to super everything out!!!11");

	//f->routine.add_node(u::block_factory[11], 0, 0);
	//f->routine.add_node(u::block_factory[4], 0, 0);

	//f->routine.connect(0, 2, 1, 0);
	//f->routine.mark(1, 0, "operand1");
	//f->routine.mark(2, 0, "operand2");
	//f->routine.mark(0, 1, "input");
	//f->routine.mark(0, 3, "yield");
	//f->update_block();

	//f->start_node = (f->routine.get_node(0));

	//u::pspec_ui(&f->routine);

	u::Graph* g = new u::Graph();

	//g->add_node(&f->represent, 0, 0, f);
	
	u::pspec_ui(g);
	delete g;
}


