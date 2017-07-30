
#include <string.h>
#include "unicorn_library.h"

#ifndef UNICORN_CFG_BLOCK_DESCRIPTIONS
#error "UNICORN_CFG_BLOCK_DESCRIPTIONS not defined"
#endif
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1
#define __DECBLOCK(cls,name,desc) { ([]() -> Block* {return new (cls)();}), name, desc},
#elif UNICORN_CFG_BLOCK_DESCRIPTIONS == 0
#define __DECBLOCK(cls,name,desc) { ([]() -> Block* {return new (cls)();}), name},
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

namespace u
{
	void AddAriphmeticBlock::work() {
		_pl_i(2) = _pl_i(0) + _pl_i(1);
	}
	void SubAriphmeticBlock::work() {
		_pl_i(2) = _pl_i(0) - _pl_i(1);
	}
	void MulAriphmeticBlock::work() {
		_pl_i(2) = _pl_i(0) * _pl_i(1);
	}
	void UMulAriphmeticBlock::work() {
		_pl_i(2) = ((unsigned)_pl_i(0) * (unsigned)_pl_i(1));
	}
	void DivAriphmeticBlock::work() {
		_pl_i(2) = _pl_i(0) / _pl_i(1);
	}

	void ReverseBitsBlock::work() {
		uint32_t x = _pl_i(0);
		uint8_t bit_count = _pl_b(1);
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
		_pl_i(2) = x;
	}

	void PulseCodeModulationBlock::work() {
		int32_t code = _pl_i(0);
		uint8_t _bit_count = _pl_b(1);
		uniseq *_sone = _pl_u(2);
		uniseq *_szero = _pl_u(3);
		uniseq *out = _pl_u(4);
		out->clear();

		for (int mask = 1 << (_bit_count - 1); mask != 0; mask >>= 1) {
			if (code & mask)
				uniseq_copy(out, _sone);
			else
				uniseq_copy(out, _szero);
		}
	}

	void PulseCountModulationBlock::work() {
		int32_t code = _pl_i(0);
		for (; code > 0; code--) {
			uniseq_copy(_pl_u(2), _pl_u(1));
		}
	}

	void PulseLengthModulationBlock::work() {
		int32_t code = _pl_i(0);
		int32_t reference = _pl_i(1);
		bool spacefirst = _pl_bool(2);
		bool significantfirst = _pl_bool(3);
		bool constsyncro = _pl_bool(4);
		uniseq *out = _pl_u(5);

		int32_t nonsignificant = constsyncro ? reference : (code - reference);

		for (; code > 0; code--) {
			uniseq_add_value(significantfirst ? code : nonsignificant, out, spacefirst ? false : true);
			uniseq_add_value(significantfirst ? nonsignificant : code, out, spacefirst ? true : false);
		}
	}

	const BlockFactory block_factory[] = {
		__DECBLOCK(Block, "", "Empty block")
		__DECBLOCK(AddAriphmeticBlock, "+", "Add")
		__DECBLOCK(SubAriphmeticBlock, "-", "Subtract")
		__DECBLOCK(MulAriphmeticBlock, "*", "Signed Multiply")
		__DECBLOCK(UMulAriphmeticBlock, "<*>", "Unsigned Multiply")
		__DECBLOCK(DivAriphmeticBlock, "/", "Divide")
		__DECBLOCK(ReverseBitsBlock, "<-", "Reverse bits")
		__DECBLOCK(PulseCodeModulationBlock, "PCM", "Pulse code modulation")
		__DECBLOCK(PulseCountModulationBlock, "P#M", "Pulse count modulation")
		__DECBLOCK(PulseLengthModulationBlock, "PLM", "Pulse width modulation")
	};

	Block* new_std_block(int factory_index) {
		Block* ret = block_factory[factory_index].construct();
		ret->factory_index = factory_index;
		if(!ret)
			return NULL;
		if (!ret->setup_ports()) {
			delete ret;
			return NULL;
		}
		return ret;
	}
}

