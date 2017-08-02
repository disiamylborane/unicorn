
#include <string.h>
#include "unicorn_library.h"

#ifndef UNICORN_CFG_BLOCK_DESCRIPTIONS
    #error "UNICORN_CFG_BLOCK_DESCRIPTIONS not defined"
#endif
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1

    #define declblock(cls,tune,pcfg,name,desc) \
        static const Block cls = \
        {work_##cls,tune_##cls,pcfg,name,desc}
        
    #define declnotune(cls,pcfg,name,desc) \
        static const Block cls = \
        {work_##cls,nullptr,pcfg,name,desc}
    
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

#define __workfunction(cls) type::n* work_##cls(port** portlist)

#define _pl_var(_type, _port) (*((type::_type*)portlist[_port]))
#define _pl_arr(_port) ((uniseq*)portlist[_port])
#define _pl_callnext(_port) return &_pl_var(n, _port)


/*
#define _pl_bool(n) (*((bool*)portlist[n]))
#define _pl_c(n) (*((char*)portlist[n]))
#define _pl_b(n) (*((uint8_t*)portlist[n]))
#define _pl_h(n) (*((int16_t*)portlist[n]))
#define _pl_i(n) (*((int32_t*)portlist[n]))
#define _pl_l(n) (*((int64_t*)portlist[n]))
#define _pl_f(n) (*((float*)portlist[n]))
#define _pl_d(n) (*((double*)portlist[n]))
#define _pl_u(n) (((uniseq*)portlist[n]))
#define _pl_s(n) (((char*)portlist[n]))
*/

namespace u
{
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
    
    static const char* ports_ArithmeticBlock = "=n=i=i|=i";
    static const char* ports_ComparisonBlock = "=n=i=i|=q";
    declnotune(AddArithmeticBlock, ports_ArithmeticBlock, "+", "Add");
    declnotune(SubArithmeticBlock, ports_ArithmeticBlock, "-", "Subtract");
    declnotune(MulArithmeticBlock, ports_ArithmeticBlock, "*", "Signed Multiply");
    declnotune(UMulArithmeticBlock,ports_ArithmeticBlock,"<*>","Uns<igned Multiply");
    declnotune(DivArithmeticBlock, ports_ArithmeticBlock, "/", "Divide");

    declnotune(ReverseBitsBlock,"=n=lx=bbitcount|=ly", "<-","Reverse bits");

    declnotune(PulseCodeModulationBlock, "=n=icode=bbitcount=Ione=Izero|=Iout", "PCM", "Pulse code modulation");
    declnotune(PulseCountModulationBlock,"=n=icode=Iseq|=Iout", "P#M", "Pulse count modulation");
    declnotune(PulseLengthModulationBlock,"=n=icode=ireference=?space first=?significant first=?const syncro|=Iout", "PLM", "Pulse width modulation");

    declnotune(BranchBlock,"=ntrue=nfalse=q?", "if", "Branch");
    declnotune(EqualBlock, ports_ComparisonBlock, "==", "Equal");
    declnotune(NotEqualBlock, ports_ComparisonBlock, "!=", "Not equal");
    declnotune(GreaterBlock, ports_ComparisonBlock, ">", "Greater");
    declnotune(LesserBlock, ports_ComparisonBlock, "<", "Lesser");
    declnotune(GreaterEqualBlock, ports_ComparisonBlock, ">=", "Greater or Equal");
    declnotune(LesserEqualBlock, ports_ComparisonBlock, "<=", "Lesser or Equal");

    const Block* block_factory[] = {
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

