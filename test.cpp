

#include "unicorn_graph.h"
#include <iostream>

using namespace u;
extern bool u::pspec_ui(u::Graph*);

int main(){
	Graph *g = new u::Graph();
	while(u::pspec_ui(g));
	delete g;
}


