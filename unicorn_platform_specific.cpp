
#include <iostream>
#include <string.h>
#include "unicorn_cfg.h"
#include "unicorn_graph.h"

#ifdef _MSC_VER
#define gets gets_s
#endif

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
		else if (strstr(s, "clear")) {
			g->~Graph();
			g = new (g) Graph;
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
			int i1, i2, i3;
			w = &s[4];
			if (sscanf(w, "%d %d %d", &i1, &i2, &i3) < 0)
				cout << "Error\n";
			else {
				g->link(i1, i2, i3);
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
		else if (strstr(s, "set ")) {
			int i1, i2, i3;
			w = &s[4];
			if (sscanf(w, "%d %d %d %d", &i1, &i2, &i3) < 0)
				cout << "Error\n";
			else {
				Node* nd = ((Node**)g->nodes->begin)[i1];
				if (get_port_type(nd, i2) == UNICORN_PORT_RIGID) {
					memcpy(nd->portlist[i2], &i3, get_type_size(get_port_symbol(nd, i2)));
				}
			}
		}
		/*else if (strstr(s, "cst ")) {
			int i1, i2, i3;
			w = &s[4];
			if (sscanf(w, "%d %d %d", &i1, &i2, &i3) < 0)
				cout << "Error\n";
			else {
				if (!g->connect_const(i1, i2, &i3))
					cout << "Can't const connect\n";
			}
		}*/
		else if (strstr(s, "run ")) {
			int i1;
			w = &s[4];
			if (sscanf(w, "%d", &i1) < 0)
				cout << "Error\n";
			else
				run_blocks(((Node**)g->nodes->begin)[i1]);
		}
		else if (strstr(s, "chk ")) {
			int i1;
			w = &s[4];
			if (sscanf(w, "%d", &i1) < 0)
				cout << "Error\n";
			else
				cout << block_factory[i1]->name << " // " << block_factory[i1]->description << endl;
		}
		else if (strstr(s, "list")) {
			cout << "\n-----------------\n";
			for (int _i = 0; _i < g->nodes->size; _i++){
				Node *i = (((Node**)g->nodes->begin)[_i]);
				cout << "  Node"  << _i << " {" << i;
				cout << "} ";
				cout << i->core->name;
				cout << " [" << i->core->description;
				cout << "]" << endl;
				for (int j = 0; j < i->ports; j++) {
					int* val = (int*)i->portlist[j];
					if (get_port_type(i, j) == UNICORN_PORT_FREE)
						cout << "    <-";
					else  // UNICORN_PORT_RIGID
						cout << "    []";
					cout << j << "  {" << val << "}" ;
					if (val) {
						cout << "[";
						char psym = get_port_symbol(i, j);
						switch (psym) {
						case UTYPE_q[0]:
							cout << *(bool*)i->portlist[j];
							break;
						case UTYPE_c[0]:
							cout << *(char*)i->portlist[j];
							break;
						case UTYPE_b[0]:
							cout << (uint32_t) *(uint8_t*)i->portlist[j];
							break;
						case UTYPE_h[0]:
							cout << *(uint16_t*)i->portlist[j];
							break;
						case UTYPE_i[0]:
							cout << *(uint32_t*)i->portlist[j];
							break;
						case UTYPE_l[0]:
							cout << *(uint64_t*)i->portlist[j];
							break;
						case UTYPE_f[0]:
							cout << *(float*)i->portlist[j];
							break;
						case UTYPE_d[0]:
							cout << *(double*)i->portlist[j];
							break;
						case UTYPE_n[0]:
							cout << (Node*)i->portlist[j];
							break;
						default:
							cout << "????????";
							break;
						}
						cout << "] " << get_type_description(psym);
					}
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

