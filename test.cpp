

#include "unicorn_graph.h"
#include <iostream>

using namespace u;
extern bool u::pspec_ui(u::Graph*);


int two = 2;
int three = 3;
int five = 5;
Graph *g;
uint8_t u8five = 5;

int main(){
	g = new u::Graph();
	
	/*	uniseq one(sizeof(u::type::i), 2);
	one.push_back_32(1200);
	one.push_back_32(600);

	uniseq zero(sizeof(u::type::i), 2);
	zero.push_back_32(600);
	zero.push_back_32(600);
	
	g->add_std_node(0,0,0);
	g->connect_const(0,1,&two);
	g->connect_const(0,2,&three);
	
	g->add_std_node(6,0,0);
	g->connect(0,3,1,1);
	g->connect_const(1, 2, &u8five);
	g->connect_const(1, 3, &one);
	g->connect_const(1, 4, &zero);
	
	g->link(0,0,1);
	
	g->run(0);*/
	
	while(u::pspec_ui(g));
	delete g;
}


