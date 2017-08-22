
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
#define ref(type,name) "\x7F" UTYPE_DIR_##type #name
#define out(type,name)  "\xFF" UTYPE_##type #name
#define param(type,name) "\xFF" UTYPE_DIR_##type #name

#define in_(type)  "\x7F" UTYPE_##type
#define ref_(type) "\x7F" UTYPE_DIR_##type
#define out_(type)  "\xFF" UTYPE_##type
#define param_(type) "\xFF" UTYPE_DIR_##type


#define __workfunction(cls) type::n* work_##cls(port** portlist)
#define __tunefunction(cls) const char* tune_##cls(port** portlist, BlockTuneType tune_type)

#define _pl_var(_type, _port) (*((type::_type*)portlist[_port]))
#define _pl_arr(_port) ((uniseq*)portlist[_port])
#define _pl_callnext(_port) return &_pl_var(n, _port)

namespace u
{
    void uniseq_timing_add_value(type::i time, uniseq *out, bool addPulse)
    {
        if (time == 0)
            return;

        type::i _len = out->size;
        bool _nowPulse = _len % 2;

        if (_len == 0) {
            if (!addPulse)
                out->push_back_32(0);
            out->push_back_32(time);
            return;
        }

        if (_nowPulse == addPulse)
            *((type::i*)out->back()) += time;
        else
            out->push_back_32(time);
    }
    void uniseq_timing_copy(uniseq *out, uniseq *in) 
    {
        int _len = in->size;
        if (_len == 0)
            return;

        int _addindex = 0;
        if (((type::i*)in->begin)[0] == 0) {
            if (_len == 1)
                return;
            _addindex = 1;
        }
        uniseq_timing_add_value(((type::i*)in->begin)[_addindex], out, !_addindex);

        out->append(in, _addindex + 1);
    }

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
    __workfunction(ShiftRightBlock)
    {
        _pl_var(i, 3) = _pl_var(i, 1) >> _pl_var(i, 2);
        _pl_callnext(0);
    }
    __workfunction(ShiftLeftBlock)
    {
        _pl_var(i, 3) = _pl_var(i, 1) << _pl_var(i, 2);
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


    __tunefunction(CircularBlock) {
		switch(tune_type)
		{
		case btt_manual:
		case btt_start:
			type::i size = _pl_var(i, 1);
			uniseq* out = _pl_arr(3);

			out->resize(size);
			memset(out->begin, 0, sizeof(type::i)*size);

			_pl_var(i, 4) = 0;
		}

        return nullptr;
    }
    __workfunction(CircularBlock) {
        type::i size = _pl_var(i, 1);
        type::i in = _pl_var(i, 2);
        uniseq* out = _pl_arr(3);
        type::i first = _pl_var(i, 4);

        ((type::i*)out->begin)[first] = in;

        first++;
        if (first == size)
            first = 0;

        _pl_var(i, 4) = first;
        _pl_callnext(0);
    }

    __workfunction(FIRBlock) {
        uniseq* fifo = _pl_arr(1);
        type::i first = _pl_var(i, 2);
        uniseq* coeffs = _pl_arr(3);

        _pl_arr(4);

        int size1 = fifo->size;
        int size2 = coeffs->size;
        int size = (size1 < size2) ? size1 : size2;

        type::i ret = 0;
        for (type::i* i = ((type::i*)coeffs->begin)+size; i > (type::i*)coeffs->begin;) {
            ret += *(--i) * ((type::i*)fifo->begin)[first++];
            if (first == size)
                first = 0;
        }
        _pl_var(i, 4) = ret;
        _pl_callnext(0);
    }
    
    static const char* ports_ArithmeticBlock = ref_(n) in_(i) in_(i) out_(i);
    static const char* ports_ComparisonBlock = ref_(n) in_(i) in_(i) out_(q);

    declconst(BoolVarBlock, "Bool", "Bool variable", param_(q));
    declconst(CharVarBlock, "Char", "Char variable", param_(c));
    declconst(ByteVarBlock, "Byte", "Byte variable", param_(b));
    declconst(ShortVarBlock, "Short", "Short variable", param_(h));
    declconst(LongVarBlock, "Long", "Long variable", param_(i));
    declconst(LongLongVarBlock, "Long Long", "64bit variable", param_(l));
    declconst(FloatVarBlock, "Float", "Float variable", param_(f));
    declconst(DoubleVarBlock, "Double", "Double variable", param_(d));
    declconst(NodeVarBlock, "Node", "Node variable", param_(n));
    declconst(TypeVarBlock, "Type", "Type variable", param_(t));

    declconst(IntArrBlock, "Uniseq/int", "Int array constant", param_(ARR_i));

    declnotune(AddArithmeticBlock, "+", "Add", ports_ArithmeticBlock);
    declnotune(SubArithmeticBlock, "-", "Subtract", ports_ArithmeticBlock);
    declnotune(MulArithmeticBlock, "*", "Signed Multiply", ports_ArithmeticBlock);
    declnotune(UMulArithmeticBlock,"<*>","Unsigned Multiply", ports_ArithmeticBlock);
    declnotune(DivArithmeticBlock, "/", "Divide", ports_ArithmeticBlock);
    declnotune(ShiftRightBlock, ">>", "Shift Right", ports_ArithmeticBlock);
    declnotune(ShiftLeftBlock, "<<", "Shift Left", ports_ArithmeticBlock);

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

    declblock(CircularBlock, "FIFO", "Circular FIFO of given size",
        ref_(n) param(i, size) in(i, in) out(ARR_i, out) out(i, first));
    declnotune(FIRBlock, "FIR", "FIR Filter working with Circular FIFO",
        ref_(n) in(ARR_i, fifo) in(i, first) in(ARR_i, coeffs) out_(i));

    const Block* block_factory[] = {
        &BoolVarBlock,
        &CharVarBlock,
        &ByteVarBlock,
        &ShortVarBlock,
        &LongVarBlock,
        &LongLongVarBlock,
        &FloatVarBlock,
        &DoubleVarBlock,
        &NodeVarBlock,
        &TypeVarBlock,
        &IntArrBlock,
        &AddArithmeticBlock,
        &SubArithmeticBlock,
        &MulArithmeticBlock,
        &UMulArithmeticBlock,
        &DivArithmeticBlock,
        &ShiftRightBlock,
        &ShiftLeftBlock,
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
        &CircularBlock,
        &FIRBlock,
    };
}

