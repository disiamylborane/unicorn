
#include <string.h>

#include "unicorn_library.h"
#include "unicorn_macro.h"


namespace u
{
	type::n* work_dummy(port** portlist) {
		return nullptr;
	}
	NodeTuneResult tune_dummy(Node* node, NodeTuneType tune_type) {
		return ntr_ok;
	}
	const char* string_dummy = "";

	NODE_WORK(AddArithmeticBlock) {
		_pl_var(i, 3) = _pl_var(i, 1) + _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(SubArithmeticBlock)
	{
		_pl_var(i, 3) = _pl_var(i, 1) - _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(MulArithmeticBlock)
	{
		_pl_var(i, 3) = _pl_var(i, 1) * _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(UMulArithmeticBlock)
	{
		_pl_var(i, 3) = ((unsigned)_pl_var(i, 1) * (unsigned)_pl_var(i, 2));
		_pl_callnext(0);
	}
	NODE_WORK(DivArithmeticBlock)
	{
		_pl_var(i, 3) = _pl_var(i, 1) / _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(ShiftRightBlock)
	{
		_pl_var(i, 3) = _pl_var(i, 1) >> _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(ShiftLeftBlock)
	{
		_pl_var(i, 3) = _pl_var(i, 1) << _pl_var(i, 2);
		_pl_callnext(0);
	}

	NODE_WORK(ReverseBitsBlock)
	{
		uint32_t x = _pl_var(i, 1);
		uint8_t bit_count = _pl_var(b, 2);
#if UNICORN_CFG_ARM_SPECIFIC_CODE == 1
		x = __RBIT(x);
#else
		uint32_t y = 0;
		for (size_t i = 0; i < sizeof(uint32_t) * 8; i++) {
			if (x & (1 << i))
				y |= 1 << (sizeof(uint32_t) * 8 - 1 - i);
		}
		x = y;
#endif
		x >>= sizeof(uint32_t) * 8 - bit_count;
		_pl_var(i, 3) = x;
		_pl_callnext(0);
	}

	NODE_WORK(EqualBlock)
	{
		_pl_var(q, 3) = _pl_var(i, 1) == _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(NotEqualBlock)
	{
		_pl_var(q, 3) = _pl_var(i, 1) != _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(GreaterBlock)
	{
		_pl_var(q, 3) = _pl_var(i, 1) > _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(LesserBlock)
	{
		_pl_var(q, 3) = _pl_var(i, 1) < _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(GreaterEqualBlock)
	{
		_pl_var(q, 3) = _pl_var(i, 1) >= _pl_var(i, 2);
		_pl_callnext(0);
	}
	NODE_WORK(LesserEqualBlock)
	{
		_pl_var(q, 3) = _pl_var(i, 1) <= _pl_var(i, 2);
		_pl_callnext(0);
	}

	NODE_WORK(BranchBlock)
	{
		type::q choose = _pl_var(q, 2);
		if (choose)
			_pl_callnext(0);
		else
			_pl_callnext(1);
	}
	NODE_WORK(RangeLoopBlock)
	{
		type::i times = _pl_var(i, 3);
		
		for (int i = 0; i < times; i++)
		{
			run_blocks(&_pl_var(n, 0));
		}
		_pl_callnext(1);
	}

	static const char* ports_ArithmeticBlock = ref_(n) in_(i) in_(i) out_(i);
	static const char* ports_ComparisonBlock = ref_(n) in_(i) in_(i) out_(q);

	static const Block lib_standard_desc[] = {
		//LIB_BLOCK_NOWORK(StdVariableBlock, "Var", "", param_(t) out_(t))

		LIB_BLOCK_NOTUNE(AddArithmeticBlock, "+", "Add", ports_ArithmeticBlock)
		LIB_BLOCK_NOTUNE(SubArithmeticBlock, "-", "Subtract", ports_ArithmeticBlock)
		LIB_BLOCK_NOTUNE(MulArithmeticBlock, "*", "Signed Multiply", ports_ArithmeticBlock)
		LIB_BLOCK_NOTUNE(UMulArithmeticBlock,"<*>","Unsigned Multiply", ports_ArithmeticBlock)
		LIB_BLOCK_NOTUNE(DivArithmeticBlock, "/", "Divide", ports_ArithmeticBlock)
		LIB_BLOCK_NOTUNE(ShiftRightBlock, ">>", "Shift Right", ports_ArithmeticBlock)
		LIB_BLOCK_NOTUNE(ShiftLeftBlock, "<<", "Shift Left", ports_ArithmeticBlock)

		LIB_BLOCK_NOTUNE(ReverseBitsBlock, "<-","Reverse bits", ref_(n) in(i,x) in(b, bitcount) out(i,y))

		LIB_BLOCK_NOTUNE(EqualBlock, "==", "Equal", ports_ComparisonBlock)
		LIB_BLOCK_NOTUNE(NotEqualBlock, "!=", "Not equal", ports_ComparisonBlock)
		LIB_BLOCK_NOTUNE(GreaterBlock, ">", "Greater", ports_ComparisonBlock)
		LIB_BLOCK_NOTUNE(LesserBlock, "<", "Lesser", ports_ComparisonBlock)
		LIB_BLOCK_NOTUNE(GreaterEqualBlock, ">=", "Greater or Equal", ports_ComparisonBlock)
		LIB_BLOCK_NOTUNE(LesserEqualBlock, "<=", "Lesser or Equal", ports_ComparisonBlock)

		LIB_BLOCK_NOTUNE(BranchBlock, "if", "Branch block", 
			ref_(n) ref_(n) in_(q))

		LIB_BLOCK_NOTUNE(RangeLoopBlock, "loop", "Loop in range",
			ref(n,bodys) ref(n,next) in(i,times) out(i,index))

	};

	extern const Library lib_standard = { "Standard",
		sizeof(lib_standard_desc) / sizeof(lib_standard_desc[0]), lib_standard_desc };

}
