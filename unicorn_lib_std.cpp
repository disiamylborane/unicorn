
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

	/*
	// char[5] is a dirty hack.
	// And node->core->ports_cfg[3] is dirty hack.
	// And magic number <5> is dirty hack.
	// The whole function is one large dirty hack.
	// TO THE GLORY OF SATAN
	struct s_var_block_internal {
		Block repr;
		char ports_cfg[5];
	};
	NODE_TUNE(StdVariableBlock) {
		switch (tune_type)
		{
		case u::ntt_manual: {

			char prevtype = node->core->ports_cfg[3];

			if (!node->internal) {
				s_var_block_internal* wbuffer;
				wbuffer = (s_var_block_internal*)U_NODE_INTERNAL_MALLOC(sizeof(s_var_block_internal));
				if (!wbuffer)
					return ntr_memory_error;
				node->internal = wbuffer;

				wbuffer->repr = *node->core;
				wbuffer->repr.ports_cfg = wbuffer->ports_cfg;

				memcpy(&wbuffer->ports_cfg, node->core->ports_cfg, 5);

				node->core = &wbuffer->repr;
			}

			//Take the value from portlist[0] and make the output port of type 
			type::t newtype = *(type::t*)node->portlist[0];
			if (((s_var_block_internal*)node->internal)->ports_cfg[3] == newtype)
				return ntr_ok;

			port** oldportlist = node->portlist; // output port
			setup_ports(node);
			memcpy(node->portlist, oldportlist, 2 * sizeof(void*) + sizeof(type::t));

			U_PORTS_FREE(oldportlist);
			break;
		}
		default:
			break;
		};
	}
	*/

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

	static const char* ports_ArithmeticBlock = ref_(n) in_(i) in_(i) out_(i);
	static const char* ports_ComparisonBlock = ref_(n) in_(i) in_(i) out_(q);

	const Block lib_standard[] = {
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
	};

	const size_t lib_standard_count = sizeof(lib_standard) / sizeof(lib_standard[0]);
}
