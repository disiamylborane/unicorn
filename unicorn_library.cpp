
#include <string.h>
#include "unicorn_library.h"

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
#define ref(type,name) "\x7F" UTYPE_RENDER_##type #name
#define out(type,name)  "\xFF" UTYPE_##type #name
#define param(type,name) "\xFF" UTYPE_RENDER_##type #name

#define in_(type)  "\x7F" UTYPE_##type
#define ref_(type) "\x7F" UTYPE_RENDER_##type
#define out_(type)  "\xFF" UTYPE_##type
#define param_(type) "\xFF" UTYPE_RENDER_##type


#define __workfunction(cls) type::n* work_##cls(port** portlist)
#define __tunefunction(cls) const char* tune_##cls(port** portlist)

#define _pl_var(_type, _port) (*((type::_type*)portlist[_port]))
#define _pl_arr(_port) ((uniseq*)portlist[_port])
#define _pl_callnext(_port) return &_pl_var(n, _port)

namespace u
{
	__workfunction(Dummy) {
		return 0;
	}
	__tunefunction(Dummy) {
		return 0;
	}

    __workfunction(AddArithmeticBlock){
        _pl_var(i, 3) = _pl_var(i, 1) + _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(SubArithmeticBlock)
    {
        _pl_var(i, 3) = _pl_var(i, 1) - _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(MulArithmeticBlock)
    {
        _pl_var(i, 3) = _pl_var(i, 1) * _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(UMulArithmeticBlock)
    {
        _pl_var(i, 3) = ((unsigned)_pl_var(i, 1) * (unsigned)_pl_var(i, 2));
        _pl_callnext(0);
    }
    __workfunction(DivArithmeticBlock)
    {
        _pl_var(i, 3) = _pl_var(i, 1) / _pl_var(i, 2);
        _pl_callnext(0);
    }

    __workfunction(ReverseBitsBlock)
    {
        uint32_t x = _pl_var(i, 1);
        uint8_t bit_count = _pl_var(b, 2);
#if UNICORN_CFG_ARM_SPECIFIC_CODE == 1
        x = __RBIT(x);
#else
        uint32_t y = 0;
        for (size_t i = 0; i < sizeof(uint32_t) * 8; i++) {
            if (x & (1 << i))
                y |= 1 << (sizeof(uint32_t)*8 - 1 - i);
        }
        x = y;
#endif
        x >>= sizeof(uint32_t) * 8 - bit_count;
        _pl_var(i, 3) = x;
        _pl_callnext(0);
    }

    __workfunction(PulseCodeModulationBlock)
    {
        int32_t code = _pl_var(i, 1);
        uint8_t _bit_count = _pl_var(b, 2);
        uniseq *_sone = _pl_arr(3);
        uniseq *_szero = _pl_arr(4);
        uniseq *out = _pl_arr(5);
        out->clear();

        for (int mask = 1 << (_bit_count - 1); mask != 0; mask >>= 1) {
            if (code & mask)
                uniseq_timing_copy(out, _sone);
            else
                uniseq_timing_copy(out, _szero);
        }
        _pl_callnext(0);
    }

    __workfunction(PulseCountModulationBlock)
    {
        int32_t code = _pl_var(i, 1);
        uniseq *out = _pl_arr(3);
        out->clear();
        
        for (; code > 0; code--) {
            uniseq_timing_copy(out, _pl_arr(2));
        }
        _pl_callnext(0);
    }

    __workfunction(PulseLengthModulationBlock)
    {
        int32_t code = _pl_var(i, 1);
        int32_t reference = _pl_var(i, 2);
        bool spacefirst = _pl_var(q, 3);
        bool significantfirst = _pl_var(q, 4);
        bool constsyncro = _pl_var(q, 5);
        uniseq *out = _pl_arr(6);

        int32_t nonsignificant = constsyncro ? reference : (code - reference);

        for (; code > 0; code--) {
            uniseq_timing_add_value(significantfirst ? code : nonsignificant, out, spacefirst ? false : true);
            uniseq_timing_add_value(significantfirst ? nonsignificant : code, out, spacefirst ? true : false);
        }
        _pl_callnext(0);
    }
    
    __workfunction(BranchBlock)
    {
        type::q choose = _pl_var(q, 2);
        if(choose)
            _pl_callnext(0);
        else
            _pl_callnext(1);
    }
    
    __workfunction(EqualBlock)
    {
        _pl_var(q, 3) =  _pl_var(i, 1) == _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(NotEqualBlock)
    {
        _pl_var(q, 3) =  _pl_var(i, 1) != _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(GreaterBlock)
    {
        _pl_var(q, 3) =  _pl_var(i, 1) > _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(LesserBlock)
    {
        _pl_var(q, 3) =  _pl_var(i, 1) < _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(GreaterEqualBlock)
    {
        _pl_var(q, 3) =  _pl_var(i, 1) >= _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(LesserEqualBlock)
    {
        _pl_var(q, 3) =  _pl_var(i, 1) <= _pl_var(i, 2);
        _pl_callnext(0);
    }
    
    static const char* ports_ArithmeticBlock = ref_(n) in_(i) in_(i) out_(i);
    static const char* ports_ComparisonBlock = ref_(n) in_(i) in_(i) out_(q);

	declconst(IntVarBlock, "Int", "Int constant", param_(i));

    declnotune(AddArithmeticBlock, "+", "Add", ports_ArithmeticBlock);
    declnotune(SubArithmeticBlock, "-", "Subtract", ports_ArithmeticBlock);
    declnotune(MulArithmeticBlock, "*", "Signed Multiply", ports_ArithmeticBlock);
    declnotune(UMulArithmeticBlock,"<*>","Uns<igned Multiply", ports_ArithmeticBlock);
    declnotune(DivArithmeticBlock, "/", "Divide", ports_ArithmeticBlock);

	declnotune(ReverseBitsBlock, "<-","Reverse bits",
		ref_(n) in(i,x) in(b, bitcount) out(i,y));
	declnotune(PulseCodeModulationBlock,"PCM", "Pulse code modulation",
		ref_(n) in(i,code) in(b,bitcount) in(ARR_i,one) in(ARR_i,zero) out(ARR_i, out));
    declnotune(PulseCountModulationBlock, "P#M", "Pulse count modulation",
		ref_(n) in(i, code) in(ARR_i, seq) out(ARR_i, out));
    declnotune(PulseLengthModulationBlock, "PLM", "Pulse width modulation",
		ref_(n) in(i,code) in(i,reference) in(q,space first) in(q,significant first) in(q,const syncro) out(ARR_i,out));

	declnotune(BranchBlock, "if", "Branch",
		ref(n,true)ref(n,false)in_(q));

	declnotune(EqualBlock, "==", "Equal", ports_ComparisonBlock);
    declnotune(NotEqualBlock, "!=", "Not equal", ports_ComparisonBlock);
    declnotune(GreaterBlock, ">", "Greater", ports_ComparisonBlock);
    declnotune(LesserBlock, "<", "Lesser", ports_ComparisonBlock);
    declnotune(GreaterEqualBlock, ">=", "Greater or Equal", ports_ComparisonBlock);
    declnotune(LesserEqualBlock, "<=", "Lesser or Equal", ports_ComparisonBlock);
	
    const Block* block_factory[] = {
		&IntVarBlock,
        &AddArithmeticBlock,
		&SubArithmeticBlock,
		&MulArithmeticBlock,
        &UMulArithmeticBlock,
        &DivArithmeticBlock,
        &ReverseBitsBlock,
        &PulseCodeModulationBlock,
        &PulseCountModulationBlock,
        &PulseLengthModulationBlock,
        &BranchBlock,
        &EqualBlock,
        &NotEqualBlock,
        &GreaterBlock,
        &LesserBlock,
        &GreaterEqualBlock,
        &LesserEqualBlock,
    };

    Node* new_std_node(int factory_index) {
        return new_node(block_factory[factory_index]);
    }
}

