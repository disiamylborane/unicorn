
#include "unicorn_library.h"
#include "unicorn_macro.h"

namespace u {
	static const Block lib_const_desc[] = {
		LIB_BLOCK_CONST(Bool, out_(q))
		LIB_BLOCK_CONST(Char, out_(c))
		LIB_BLOCK_CONST(Byte, out_(b))
		LIB_BLOCK_CONST(Short, out_(h))
		LIB_BLOCK_CONST(Long, out_(i))
		LIB_BLOCK_CONST(LongLong, out_(l))
		LIB_BLOCK_CONST(Float, out_(f))
		LIB_BLOCK_CONST(Double, out_(d))
		LIB_BLOCK_CONST(Node, out_(n))
		LIB_BLOCK_CONST(Type, out_(t))

		LIB_BLOCK_CONST(<Bool>, out_(ARR_q))
		LIB_BLOCK_CONST(<Char>, out_(ARR_c))
		LIB_BLOCK_CONST(<Byte>, out_(ARR_b))
		LIB_BLOCK_CONST(<Short>, out_(ARR_h))
		LIB_BLOCK_CONST(<Long>, out_(ARR_i))
		LIB_BLOCK_CONST(<LongLong>, out_(ARR_l))
		LIB_BLOCK_CONST(<Float>, out_(ARR_f))
		LIB_BLOCK_CONST(<Double>, out_(ARR_d))
		LIB_BLOCK_CONST(<Node>, out_(ARR_n))
		LIB_BLOCK_CONST(<Type>, out_(ARR_t))
	};

	extern const Library lib_const = {  "Const", sizeof(lib_const_desc) / sizeof(lib_const_desc[0]), lib_const_desc };
}
