

#include "unicorn_graph.h"
#include <iostream>

extern bool u::pspec_ui(u::Graph*);

int main(){
	u::Graph* g = new u::Graph();
	
	while(u::pspec_ui(g));
	delete g;
}


