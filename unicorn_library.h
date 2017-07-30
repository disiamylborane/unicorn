
#ifndef UNICORN_LIBRARY_H_GUARD
#define UNICORN_LIBRARY_H_GUARD

#include "unicorn_cfg.h"
#include "unicorn_block.h"

namespace u
{

	struct BlockFactory {
		Block* (*construct)();
		const char* name;
#if UNICORN_CFG_BLOCK_DESCRIPTIONS == 1
		const char* description;
#endif
	};
	extern const BlockFactory block_factory[];

	//bool construct_block(Block* out);
	//void deconstruct_block(Block* bl);
	Block* new_std_block(int factory_index);

	class AriphmeticBlock : public Block {
		virtual const char* ports_cfg() override
		{return "=l=l|=l";}
	};
	class AddAriphmeticBlock : public AriphmeticBlock {
		virtual void work() override;
	};
	class SubAriphmeticBlock : public AriphmeticBlock {
		virtual void work() override;
	};
	class MulAriphmeticBlock : public AriphmeticBlock {
		virtual void work() override;
	};
	class UMulAriphmeticBlock : public AriphmeticBlock {
		virtual void work() override;
	};
	class DivAriphmeticBlock : public AriphmeticBlock {
		virtual void work() override;
	};

	class ReverseBitsBlock : public Block {
		virtual const char* ports_cfg() override
		{return "=lx=bbitcount|=ly";}
		virtual void work() override;
	};

	class PulseCodeModulationBlock : public Block {
		virtual const char* ports_cfg() override
		{return "=icode=bbitcount=uone=uzero|=uout";}
		virtual void work() override;
	};
	class PulseCountModulationBlock : public Block {
		virtual const char* ports_cfg() override
		{return "=icode=useq|=uout";}
		virtual void work() override;
	};
	class PulseLengthModulationBlock : public Block {
		virtual const char* ports_cfg() override
		{return "=icode=ireference=?space first=?significant first=?const syncro|=uout";}
		virtual void work() override;
	};
}

#endif