
#include <new>

#include "unicorn_graph.h"

extern void u::pspec_ui(u::Graph*);
extern u::uniseq global_functions;

int main(){
	new (&global_functions) u::uniseq(sizeof(u::Function*), 4);

	u::Graph* g = new u::Graph();
	
	u::pspec_ui(g);

	global_functions.~uniseq();
	delete g;
}


