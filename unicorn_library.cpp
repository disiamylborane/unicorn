
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

#define __workfunction(cls) void work_##cls(port** portlist)

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
	__workfunction(AddArithmeticBlock){
		_pl_i(2) = _pl_i(0) + _pl_i(1);
	}
	__workfunction(SubArithmeticBlock)
	{
		_pl_i(2) = _pl_i(0) - _pl_i(1);
	}
	__workfunction(MulArithmeticBlock)
	{
		_pl_i(2) = _pl_i(0) * _pl_i(1);
	}
	__workfunction(UMulArithmeticBlock)
	{
		_pl_i(2) = ((unsigned)_pl_i(0) * (unsigned)_pl_i(1));
	}
	__workfunction(DivArithmeticBlock)
	{
		_pl_i(2) = _pl_i(0) / _pl_i(1);
	}

	__workfunction(ReverseBitsBlock)
	{
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

	__workfunction(PulseCodeModulationBlock)
	{
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

	__workfunction(PulseCountModulationBlock)
	{
		int32_t code = _pl_i(0);
		for (; code > 0; code--) {
			uniseq_copy(_pl_u(2), _pl_u(1));
		}
	}

	__workfunction(PulseLengthModulationBlock)
	{
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
	
	
	static const char* ports_ArithmeticBlock = "=l=l|=l";
	declnotune(AddArithmeticBlock, ports_ArithmeticBlock, "+", "Add");
	declnotune(SubArithmeticBlock, ports_ArithmeticBlock, "-", "Subtract");
	declnotune(MulArithmeticBlock, ports_ArithmeticBlock, "*", "Signed Multiply");
	declnotune(UMulArithmeticBlock,ports_ArithmeticBlock,"<*>","Uns<igned Multiply");
	declnotune(DivArithmeticBlock, ports_ArithmeticBlock, "/", "Divide");

	declnotune(ReverseBitsBlock,"=lx=bbitcount|=ly", "<-","Reverse bits");

	declnotune(PulseCodeModulationBlock, "=icode=bbitcount=uone=uzero|=uout", "PCM", "Pulse code modulation");
	declnotune(PulseCountModulationBlock,"=icode=useq|=uout", "P#M", "Pulse count modulation");
	declnotune(PulseLengthModulationBlock,"=icode=ireference=?space first=?significant first=?const syncro|=uout", "PLM", "Pulse width modulation");

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
	};

	Node* new_std_node(int factory_index) {
		Node* ret = (Node*)U_MALLOC(sizeof(Node));
		ret->core = block_factory[factory_index];
		if(!ret)
			return NULL;
		if (!ret->setup_ports()) {
			delete ret;
			return NULL;
		}
		return ret;
	}
}

