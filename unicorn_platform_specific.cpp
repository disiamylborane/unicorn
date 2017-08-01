
#include <iostream>
#include <string.h>
#include "unicorn_graph.h"


namespace u {

	char s[256];
	char *w;

	short curx, cury;


	bool pspec_ui(Graph *g) {
		gets(s);
		curx = cury=0;

		if (strstr(s, "add ")) {
			w = &s[4];
			int num = atoi(w);
			g->add_std_node(num, curx, cury);
		}
		else if (strstr(s, "cur ")) {
			int i1, i2;
			w = &s[4];
			if (sscanf(w, "%d %d", &i1, &i2) < 0)
				cout << "Error\n";
			else {
				curx = i1;
				cury = i2;
			}
		}
		else if (strstr(s, "lnk ")) {
			int i1, i2;
			w = &s[4];
			if (sscanf(w, "%d %d", &i1, &i2) < 0)
				cout << "Error\n";
			else {
				g->link(i1,i2);
			}
		}
		else if (strstr(s, "cnt ")) {
			int i1, i2, i3, i4;
			w = &s[4];
			if (sscanf(w, "%d %d %d %d", &i1, &i2, &i3, &i4) < 0)
				cout << "Error\n";
			else {
				if(!g->connect(i1, i2, i3, i4))
					cout << "Can't connect\n";
			}
		}
		else if (strstr(s, "cst ")) {
			int i1, i2, i3;
			w = &s[4];
			if (sscanf(w, "%d %d %d", &i1, &i2, &i3) < 0)
				cout << "Error\n";
			else {
				if (!g->connect_const(i1, i2, &i3))
					cout << "Can't const connect\n";
			}
		}
		else if (strstr(s, "run ")) {
			int i1;
			w = &s[4];
			if (sscanf(w, "%d", &i1) < 0)
				cout << "Error\n";
			else
				run_blocks((*g->nodes)[i1]);
		}
		else if (strstr(s, "chk ")) {
			int i1;
			w = &s[4];
			if (sscanf(w, "%d", &i1) < 0)
				cout << "Error\n";
			else
				cout << block_factory[i1]->name << " " << block_factory[i1]->description << endl;
		}
		else if (strstr(s, "list")) {
			cout << "-----------------\n";
			for (auto i : *g->nodes) {
				cout << "  Node" << i->core->ports_cfg << " {" << i;
				cout << "} " << i->core->name << " [" << i->core->description;
				cout << "] Linked to " << i->next << endl;
				int inputs = i->inputs;
				for (int j = 0; j < inputs; j++) {
					int* val = (int*)i->portlist[j];
					cout << "    in" << j << "  {" << val << "} [" ;
					if (val)
						cout << *(int*)i->portlist[j];
					cout << endl;
				}
				for (int j = 0; j < i->outputs; j++) {
					int* val = (int*)i->portlist[j + inputs];
					cout << "    out" << j << " [" << val << "] ";					if (val)
						cout << *val;
					cout << endl;
				}
			}
		}
		else if (strstr(s, "exit")) {
			return false;
		}
		return true;
	}
}

