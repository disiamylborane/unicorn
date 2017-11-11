#pragma once

#include "unicorn_cfg.h"


#ifndef UNICORN_CFG_TYPE_DESCRIPTIONS
#error "UNICORN_CFG_TYPE_DESCRIPTIONS not defined"
#endif
#if UNICORN_CFG_TYPE_DESCRIPTIONS == 1
#define __DECLARE_UNICORN_TYPE(sym,size,desc) {sym, size, desc},
#elif UNICORN_CFG_TYPE_DESCRIPTIONS == 0
#define __DECLARE_UNICORN_TYPE(sym,size,desc) {sym, size},
#else 
#error "Wrong UNICORN_CFG_TYPE_DESCRIPTIONS parameter"
#endif



#ifndef UNICORN_CFG_BLOCK_DESCRIPTIONS
#error "UNICORN_CFG_BLOCK_DESCRIPTIONS not defined"
#endif
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1
#define declblock(cls,name,desc,pcfg) \
        static const Block cls = \
        {work_##cls,tune_##cls,pcfg,name,desc}

#define declnotune(cls,name,desc,pcfg) \
        static const Block cls = \
        {work_##cls,tune_Dummy,pcfg,name,desc}

#define declnowork(cls,name,desc,pcfg) \
        static const Block cls = \
        {work_Dummy,tune_##cls,pcfg,name,desc}

#define declconst(cls,name,desc,pcfg) \
        static const Block cls = \
        {work_Dummy,tune_Dummy,pcfg,name,desc}

#define LIB_BLOCK(cls,name,desc,pcfg) \
	{ work_##cls, tune_##cls, pcfg, name, desc },
#define LIB_BLOCK_NOWORK(cls,name,desc,pcfg) \
	{ work_dummy, tune_##cls, pcfg, name, desc },
#define LIB_BLOCK_NOTUNE(cls,name,desc,pcfg) \
	{ work_##cls, tune_dummy, pcfg, name, desc },
#define LIB_BLOCK_CONST(cls, pcfg) \
	{ work_dummy, tune_dummy, pcfg, #cls, string_dummy },

#elif UNICORN_CFG_BLOCK_DESCRIPTIONS == 0

#define declblock(cls,work,tune,pcfg,name,desc) \
        static const Block cls = \
        {work_##cls,tune_##cls,pcfg,name}

#define declnotune(cls,pcfg,name,desc) \
        static const Block cls = \
        {work_##cls,nullptr,pcfg,name}

#else
#error "Wrong UNICORN_CFG_BLOCK_DESCRIPTIONS parameter"
#endif

#ifndef UNICORN_CFG_ARM_SPECIFIC_CODE
#error "UNICORN_CFG_ARM_SPECIFIC_CODE not defined"
#endif
#if UNICORN_CFG_ARM_SPECIFIC_CODE == 1
#elif UNICORN_CFG_ARM_SPECIFIC_CODE == 0
#else 
#error "Wrong UNICORN_CFG_ARM_SPECIFIC_CODE parameter"
#endif

#define in(type,name)  "\x7F" UTYPE_##type #name
#define ref(type,name) "\x7F" UTYPE_DIR_##type #name
#define out(type,name)  "\xFF" UTYPE_##type #name
#define param(type,name) "\xFF" UTYPE_DIR_##type #name

#define in_(type)  "\x7F" UTYPE_##type
#define ref_(type) "\x7F" UTYPE_DIR_##type
#define out_(type)  "\xFF" UTYPE_##type
#define param_(type) "\xFF" UTYPE_DIR_##type

#define __workfunction(cls) type::n* work_##cls(port** portlist)
#define __tunefunction(cls) NodeTuneResult tune_##cls(Node* node, NodeTuneType tune_type)

#define NODE_WORK(cls) static type::n* work_##cls(port** portlist)
#define NODE_TUNE(cls) static NodeTuneResult tune_##cls(Node* node, NodeTuneType tune_type)

#define _pl_var(_type, _port) (*((type::_type*)portlist[_port]))
#define _pl_arr(_port) ((uniseq*)portlist[_port])
#define _pl_callnext(_port) return &_pl_var(n, _port)

#define _pl_tn_var(_type, _port) (*((type::_type*)node->portlist[_port]))
#define _pl_tn_arr(_port) ((uniseq*)node->portlist[_port])

