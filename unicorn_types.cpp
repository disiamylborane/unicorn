
#include "unicorn_types.h"

#ifndef UNICORN_CFG_TYPE_DESCRIPTIONS
#error "UNICORN_CFG_TYPE_DESCRIPTIONS not defined"
#endif
#if UNICORN_CFG_TYPE_DESCRIPTIONS == 1
#define __DECLARE_UNICORN_TYPE(sym,size,desc) {sym, size, desc},
#elif UNICORN_CFG_TYPE_DESCRIPTIONS == 0
#define __DECLARE_UNICORN_TYPE(sym,size,desc) {sym, size},
#else 
#error "Wrong UNICORN_CFG_TYPE_DESCRIPTIONS parameter"
#endif

namespace u
{

	TypeDescriptor types[] = {
		__DECLARE_UNICORN_TYPE('q', sizeof(bool), "Bool")
		__DECLARE_UNICORN_TYPE('c', sizeof(char), "Char")
		__DECLARE_UNICORN_TYPE('b', sizeof(uint8_t), "Byte")
		__DECLARE_UNICORN_TYPE('h', sizeof(uint16_t), "Short")
		__DECLARE_UNICORN_TYPE('i', sizeof(uint32_t), "Int")
		__DECLARE_UNICORN_TYPE('l', sizeof(uint64_t), "Long Long")
		__DECLARE_UNICORN_TYPE('f', sizeof(float), "Float")
		__DECLARE_UNICORN_TYPE('d', sizeof(double), "Double")
	};

	TypeDescriptor arrays[] = {
		__DECLARE_UNICORN_TYPE('s', 4, "String")
		__DECLARE_UNICORN_TYPE('u', 4, "Uniseq")
	};

	uint8_t get_type_size(char symbol)
	{
		for (auto i : types) {
			if (i.symbol == symbol)
				return i.size;
		}
		return 0;
	}

	bool is_array_type(char symbol) {
		for (auto i : arrays) {
			if (i.symbol == symbol)
				return true;
		}
		return false;
	}

	void* new_array_type(char symbol) {
		switch (symbol) {
		case 'u': {
			uniseq *ret = new uniseq;
			ret->reserve(UNICORN_CFG_UNISEQ_INITIAL_RESERVE);
			return ret;
		}
		default:
			return 0;
		}
	}

	void delete_array_type(char symbol, void* arr) {
		switch (symbol) {
		case 'u':
			delete (uniseq*)arr;
			return;
		default:
			return;
		}
	}

	void uniseq_add_value(unidata time, uniseq *out, bool addPulse) {
		unidata _len = out->size();
		bool _nowPulse = _len % 2 == 0;

		if (time == 0)
			return;

		if (_len == 0) {
			if (!addPulse)
				out->push_back(0);
			out->push_back(time);
			return;
		}

		if (_nowPulse == addPulse)
			out->back() += time;
		else 
			out->push_back(time);
	}

	void uniseq_copy(uniseq *out, uniseq *in) {
		int _len = in->size();
		if (_len == 0)
			return;

		int _addindex = 0;
		if ((*in)[0]) {
			if(_len==1)
				return;
			_addindex = 1;
		}
		uniseq_add_value((*in)[_addindex], out, _addindex);

		for (;_addindex < _len; _addindex++) {
			out->push_back((*in)[_addindex]);
		}
	}


}
