
#ifndef UNICORN_CFG_H_GUARD
#define UNICORN_CFG_H_GUARD

#include <malloc.h>

#define UNICORN_CFG_TYPE_DESCRIPTIONS 1
#define UNICORN_CFG_BLOCK_DESCRIPTIONS 1

#define UNICORN_CFG_PLATFORM_ALIGN 4

#define UNICORN_CFG_uniseq_INITIAL_RESERVE 4
#define UNICORN_CFG_uniseq_BLOCK_RESERVE 4
#define UNICORN_CFG_uniseq_CONST_RESERVE 4
#define UNICORN_CFG_UNISEQ_INITIAL_RESERVE 40

#define UNICORN_CFG_ARM_SPECIFIC_CODE 0

#define U_PORTS_MALLOC malloc
#define U_PORTS_FREE free

#define U_NODE_MALLOC malloc
#define U_NODE_FREE free

#define U_GRAPH_MALLOC malloc
#define U_GRAPH_FREE free

#define U_ARRAY_MALLOC malloc
#define U_ARRAY_REALLOC realloc
#define U_ARRAY_FREE free

//======================================================

/*
	There are 4 Node port types:
		Input  - a port that can be connected to data buffer and read smth from it
		Output - a port which the buffer is allocated for. After <working>, the
			Node writes smth to it.
		Output by reference - a port that can be connected to data buffer and write to it
		Settings - a port which the buffer is allocated for; usually read-only. It
			is used to tweak the node behaviour without using external constants.
		
		There is no implementation difference between Input/Reference and Output/Settings,
		but it hints user/software how to use and display it.
		
Ports_cfg format:
	<special_symbol> [data_type_symbol] ...name...
	
	special_symbol:
		0x7F means "input port" or "output by reference". The Node class doesn't
			allocate memory for this port, data are external
		0xFF means "output port" or "settings port". The Node class allocates
			memory, the data are internal and can be shared to other nodes.
	
	[data_type_symbol]:
		bit 7: if set, the port is Reference/Settings. If reset, the port is Input/Output
		bit 6: if set, the type is array(uniseq). If reset, the type is single variable
		bits 5:0: data type mark. Refer to UTYPE_xxx defines in unicorn_types.h
*/

#define UNICORN_PORT_ERROR '\0'
#define UNICORN_PORT_FREE  '\x7F'
#define UNICORN_PORT_RIGID '\xFF'

#define UNICORN_SYMBOL_IS_PORT(c) ((c | (char)0x80) == (char)0xFF)

#endif
