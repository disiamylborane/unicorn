
#ifndef UNICORN_CFG_H_GUARD
#define UNICORN_CFG_H_GUARD

#include <malloc.h>

#define UNICORN_CFG_TYPE_DESCRIPTIONS 1
#define UNICORN_CFG_BLOCK_DESCRIPTIONS 1

#define UNICORN_CFG_BLOCK_DECOUPLER '|'
#define UNICORN_CFG_BLOCK_TYPEMARK '='

#define UNICORN_CFG_PLATFORM_ALIGN 4

#define UNICORN_CFG_uniseq_INITIAL_RESERVE 4
#define UNICORN_CFG_uniseq_BLOCK_RESERVE 4
#define UNICORN_CFG_uniseq_CONST_RESERVE 4
#define UNICORN_CFG_UNISEQ_INITIAL_RESERVE 40

#define UNICORN_CFG_ARM_SPECIFIC_CODE 0

#define U_MALLOC malloc
#define U_REALLOC realloc
#define U_FREE free

#define U_ARRAY_MALLOC malloc
#define U_ARRAY_REALLOC realloc
#define U_ARRAY_FREE free

#endif
