
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>

#include "unicorn_library.h"
#include "unicorn_graph.h"

#ifdef _MSC_VER
#define gets gets_s
#endif

u::uniseq global_functions;
using std::cout;
using std::cin;
using std::endl;

namespace u {

    //const Block* libs[] = { lib_const, lib_standard, lib_linecode };
    //const size_t libcounts[] = { lib_const_count, lib_standard_count, lib_linecode_count };

    char s[256];
    char *w;

    short curx, cury;

    void cout_value(void* val, char type) {
        switch (type) {
        case UTYPE_q[0]:
            cout << *(bool*)val;
            break;
        case UTYPE_c[0]:
            cout << *(char*)val;
            break;
        case UTYPE_b[0]:
            cout << (uint32_t)*(uint8_t*)val;
            break;
        case UTYPE_h[0]:
            cout << *(int16_t*)val;
            break;
        case UTYPE_i[0]:
            cout << *(int32_t*)val;
            break;
        case UTYPE_l[0]:
            cout << *(int64_t*)val;
            break;
        case UTYPE_f[0]:
            cout << *(float*)val;
            break;
        case UTYPE_d[0]:
            cout << *(double*)val;
            break;
        case UTYPE_n[0]:
            cout << (Node*)val;
            break;
        case UTYPE_t[0]:
            cout << types[*(type::t*)val].description;
            break;
        default:
            cout << "?";
            break;
        }
    }
    void* make_value(const char* s, char type) {
        uint8_t sz = get_type_size(type);
        if (!sz)
            return nullptr;
        void* val = malloc(sz);
        switch (type) {
        case UTYPE_q[0]:
            *(bool*)val = !!atoi(s);
            break;
        case UTYPE_c[0]:
            *(char*)val = atoi(s);
            break;
        case UTYPE_b[0]:
            *(uint8_t*)val = atoi(s);
            break;
        case UTYPE_h[0]:
            *(uint16_t*)val = atoi(s);
            break;
        case UTYPE_i[0]:
            *(uint32_t*)val = atoi(s);
            break;
        case UTYPE_l[0]:
            *(uint64_t*)val = atoi(s);
            break;
        case UTYPE_f[0]:
            *(float*)val = (float)atof(s);
            break;
        case UTYPE_d[0]:
            *(double*)val = atof(s);
            break;
        case UTYPE_t[0]:
            *(type::t*)val = atoi(s);
            break;
        default:
            free(val);
            return nullptr;
        }
        return val;
    }

    void print_port_name(const char* name)
    {
        while (*name && !(UNICORN_SYMBOL_IS_PORT(*name)))
        {
            cout << *name;
            name++;
        }
    }

    void pspec_ui(Graph *g) {
		int curr_lib = 0;
		
        while (1) {
            gets(s);
            curx = cury = 0;

			if (strstr(s, "lib") == s) {
				int i1;
				w = &s[3];
				if (sscanf(w, " %d", &i1) < 1)
					cout << "Library " << curr_lib << ": " << libraries[curr_lib].name << endl;
				else {
					if (i1 >= libraries_count) {
						cout << "No library " << i1 << endl;
					}
					curr_lib = i1;
				}
			}
			else if (strstr(s, "add ") == s) {
				int i1;
				w = &s[4];
				if (sscanf(w, "%d", &i1) < 1)
					cout << "Error\n";
				else {
					if (i1 >= libraries[curr_lib].count) {
						cout << "No function " << i1 << " in library" << curr_lib << endl;
					}
					g->add_node(&libraries[curr_lib].blocks[i1], curx, cury);
				}
			}
			else if (strstr(s, "del ") == s) {
				w = &s[4];
				uintmax_t num = strtoumax(w, nullptr, 10);
				if (num == UINTMAX_MAX && errno == ERANGE) {
					cout << "Error\n";
				}
				else {
					g->del_node(num);
				}
			}
			else if (strstr(s, "clear") == s) {
				g->clear();
			}
			else if (strstr(s, "cur ") == s) {
				int i1, i2;
				w = &s[4];
				if (sscanf(w, "%d %d", &i1, &i2) < 2)
					cout << "Error\n";
				else {
					curx = i1;
					cury = i2;
				}
			}
            else if (strstr(s, "link ") == s) {
                int i1, i2, i3;
                w = &s[5];
                if (sscanf(w, "%d %d %d", &i1, &i2, &i3) < 3)
                    cout << "Error\n";
                else {
                    g->link(i1, i2, i3);
                }
            }
            else if (strstr(s, "ulink ") == s) {
                int i1, i2;
                w = &s[6];
                if (sscanf(w, "%d %d", &i1, &i2) < 2)
                    cout << "Error\n";
                else {
                    g->unlink(i1, i2);
                }
            }
            else if (strstr(s, "mark ") == s) {
                int i1, i2;
                char mktext[64];
                w = &s[5];
                if (sscanf(w, "%d %d %s", &i1, &i2, mktext) < 2)
                    cout << "Error\n";
                else {
                    g->mark(i1, i2, mktext);
                }
            }
            else if (strstr(s, "umark ") == s) {
                int i1;
                w = &s[6];
                if (sscanf(w, "%d", &i1) < 1)
                    cout << "Error\n";
                else {
                    g->unmark(i1);
                }
            }
            else if (strstr(s, "cnt ")) {
                int i1, i2, i3, i4;
                w = &s[4];
                if (sscanf(w, "%d %d %d %d", &i1, &i2, &i3, &i4) < 4)
                    cout << "Error\n";
                else {
                    if (!g->connect(i1, i2, i3, i4))
                        cout << "Can't connect\n";
                }
            }
            else if (strstr(s, "ucnt ")) {
                int i1, i2;
                w = &s[5];
                if (sscanf(w, "%d %d", &i1, &i2) < 2)
                    cout << "Error\n";
                else {
                    g->disconnect(i1, i2);
                }
            }
            else if (strstr(s, "set ") == s) {
                int i1, i2;
                w = &s[4];
                char __rest[1024];
                char *rest = __rest;
                if (sscanf(w, "%d %d %s", &i1, &i2, rest) < 3)
                    cout << "Error\n";
                else {
                    Node* nd = ((Node**)g->node_buffer->begin)[i1];
                    PortDefinition pd = node_port_get_definition(nd->core, i2);
                    if (node_port_get_location(pd) == pl_internal) {
                        char psym = node_port_get_datatype(pd);
                        if (is_array_type(psym))
                        {
                            uniseq* seq = ((uniseq*)nd->portlist[i2]);
                            seq->clear();
                            psym = get_arr_type(psym);
                            for (;;) {
                                if (!(*rest)) {
                                    break;
                                }
                                void* val = make_value(rest, psym);
                                seq->push_back(val);
                                free(val);
                                do {
                                    rest++;
                                } while ((rest[-1] != ',') && (*rest));
                            }
                        }
                        else {
                            void* val = make_value(rest, psym);
                            memcpy(nd->portlist[i2], val, get_type_size(psym));
                            free(val);
                        }
                    }
                }
            }
            else if (strstr(s, "ready") == s) {
                g->ready();
            }
            else if (strstr(s, "tune ") == s) {
                int i1;
                w = &s[5];
                if (sscanf(w, "%d", &i1) < 1)
                    cout << "Error\n";
                else
                    g->tune_node(i1);
            }
            else if (strstr(s, "run ") == s) {
                int i1;
                w = &s[4];
                if (sscanf(w, "%d", &i1) < 1)
                    cout << "Error\n";
                else
                    run_blocks(((Node**)g->node_buffer->begin)[i1]);
            }
            else if (strstr(s, "chk") == s) {
		int i1;
		w = &s[3];
		if (sscanf(w, " %d", &i1) == 1) {
			if (i1 >= libraries_count)
				cout << "No library " << i1<< endl;
			else for (int i = 0; i < libraries[i1].count; i++) {
				cout << i << ". [" << libraries[i1].blocks[i].name << "] " << libraries[i1].blocks[i].description << endl;
			}
		}
		else {
			cout << "Error" << endl;
		}
            }
            else if (strstr(s, "fchk") == s) {
                for (int i = 0; i < global_functions.size; i++) {
                    Block* b = &((Function**)global_functions.begin)[i]->represent;
                    cout << "Function" << i;
                    if (b->name)
                        cout << ". [" << (char*)b->name << "] ";
                    if (b->description)
                        cout << b->description << endl;
                }
            }
            else if (strstr(s, "list") == s) {
                cout << "\n-----------------\n";
                for (int _i = 0; _i < g->node_buffer->size; _i++) {
                    Node *i = (((Node**)g->node_buffer->begin)[_i]);
                    cout << "  Node" << _i << " {" << i;
                    cout << "} ";
                    cout << i->core->name;
                    cout << " [" << i->core->description;
                    cout << "]" << endl;
                    for (int j = 0; j < i->ports; j++) {
                        int* val = (int*)i->portlist[j];
                        PortDefinition pd = node_port_get_definition(i->core, j);
                        if (node_port_get_location(pd) == pl_internal) {
                            if (node_port_get_direction(pd) == pd_output)
                                cout << "  []";
                            else
                                cout << "  **";
                        }
                        else {  // UNICORN_PORT_INTERNAL
                            if (node_port_get_direction(pd) == pd_output)
                                cout << "  <=";
                            else
                                cout << "  ->";
                        }
                        cout << j << "  {" << val << "} ";
                        char psym = node_port_get_datatype(pd);
                        cout << get_type_description(psym) << ' ';
                        print_port_name(node_port_get_name(pd));
                        if (val) {
                            cout << " [";
                            if (is_array_type(psym)) {
                                for (int k = 0; k < ((uniseq*)i->portlist[j])->size; k++) {
                                    cout_value(((uniseq*)i->portlist[j])->at(k), psym & 0x3F);
                                    cout << ",";
                                }
                            }
                            else
                                cout_value(i->portlist[j], psym);
                            cout << "]";
                        }
                        int marki = g->_port_marked(_i, j);
                        if (marki != -1)
                            cout << " marked [" << marki << "] as " << (char*)(((Mark*)(g->mark_buffer->begin))[marki].name->begin);
                        cout << endl;
                    }
                }
            }
            else if (strstr(s, "def ") == s) {
                int i1;
                char name[20];
                w = &s[4];
                if (sscanf(w, "%s", name) < 1)
                    cout << "Error\n";
                else {
                    Function *f = new u::Function(name);
                    pspec_ui(&f->routine);
                    cout << "Start node:\n";
                    cin >> i1;
                    f->start_node = ((Node**)f->routine.node_buffer->begin)[i1];
                    f->update_block();
                    global_functions.push_back_pointer(f);
                }
            }
            else if (strstr(s, "call ") == s) {
                int i1;
                w = &s[5];
                if (sscanf(w, "%i", &i1) < 1)
                    cout << "Error\n";
                else {
                    if (global_functions.size > i1) {
                        Function *f = ((Function**)global_functions.begin)[i1];
                        g->add_node(&f->represent, 0, 0, f);
                    }
                    else
                        cout << "Function " << i1 << " not defined";
                }
            }
            else if (strstr(s, "edit ") == s) {
                int i1;
                w = &s[5];
                if (sscanf(w, "%i", &i1) < 1)
                    cout << "Error\n";
                else {
                    Function *f = *(Function **)global_functions.at(i1);
                    pspec_ui(&f->routine);
                    cout << "Start node:\n";
                    cin >> i1;
                    f->start_node = ((Node**)f->routine.node_buffer->begin)[i1];
                    if(f->update_block()){
                        for (int i = 0; i < g->node_buffer->size; i++) {
                            Node* cn = ((Node**)g->node_buffer->begin)[i];
                            if (cn->core == &f->represent) {
                                g->del_node(i);
                                i--;
                            }
                        }
                    }
                }
            }
            else if (strstr(s, "save ") == s) {
                int i1;
                w = &s[5];
                if (strlen(w) < 1)
                    cout << "Error\n";
                else {
                    int fd = open(w, O_WRONLY | O_CREAT, 0666);
                    g->save_file(fd);
                    close(fd);
                }
            }
			else if (strstr(s, "load ") == s) {
				int i1;
				w = &s[5];
				if (strlen(w) < 1)
					cout << "Error\n";
				else {
					int fd = open(w, O_RDONLY, 0666);
					bool result = g->load_file(fd);
					close(fd);
					if (!result)
						cout << "Error loading file";
				}
			}
            else if (strstr(s, "exit") == s) {
                return;
            }
        }
    }
}

