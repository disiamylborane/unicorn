
#include "unicorn_library.h"
#include "unicorn_macro.h"

namespace u {
	const Block lib_const[] = {
		LIB_BLOCK_CONST(Bool, param_(q))
		LIB_BLOCK_CONST(Char, param_(c))
		LIB_BLOCK_CONST(Byte, param_(b))
		LIB_BLOCK_CONST(Short, param_(h))
		LIB_BLOCK_CONST(Long, param_(i))
		LIB_BLOCK_CONST(LongLong, param_(l))
		LIB_BLOCK_CONST(Float, param_(f))
		LIB_BLOCK_CONST(Double, param_(d))
		LIB_BLOCK_CONST(Node, param_(n))
		LIB_BLOCK_CONST(Type, param_(t))

		LIB_BLOCK_CONST(<Bool>, param_(ARR_q))
		LIB_BLOCK_CONST(<Char>, param_(ARR_c))
		LIB_BLOCK_CONST(<Byte>, param_(ARR_b))
		LIB_BLOCK_CONST(<Short>, param_(ARR_h))
		LIB_BLOCK_CONST(<Long>, param_(ARR_i))
		LIB_BLOCK_CONST(<LongLong>, param_(ARR_l))
		LIB_BLOCK_CONST(<Float>, param_(ARR_f))
		LIB_BLOCK_CONST(<Double>, param_(ARR_d))
		LIB_BLOCK_CONST(<Node>, param_(ARR_n))
		LIB_BLOCK_CONST(<Type>, param_(ARR_t))
	};

	const size_t lib_const_count = sizeof(lib_const) / sizeof(lib_const[0]);
}
