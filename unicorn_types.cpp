#include <ctype.h>
#include <string.h>
#include "unicorn_types.h"
#include "unicorn_block.h"

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
	const TypeDescriptor types[] = {
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(q), sizeof(type::q), "Bool")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(c), sizeof(type::c), "Char")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(b), sizeof(type::b), "Byte")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(h), sizeof(type::h), "Short")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(i), sizeof(type::i), "Long")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(l), sizeof(type::l), "Long Long")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(f), sizeof(type::f), "Float")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(d), sizeof(type::d), "Double")
		__DECLARE_UNICORN_TYPE(CHAR_TYPE(n), sizeof(type::n), "Node")
	};

	uint8_t get_type_size(char symbol)
	{
		for (auto i : types) {
			if (i.symbol == symbol)
				return i.size;
		}
		return 0;
	}

	const char* get_type_description(char symbol)
	{
		for (auto i : types) {
			if (i.symbol == symbol)
				return i.description;
		}
		return 0;
	}
	
	uint8_t get_arr_size(char symbol)
	{
		if(!(symbol & 0x40))
			return 0;
		return get_type_size(symbol & (~0x40));
	}

	bool is_array_type(char symbol) {
		if (!(symbol & 0x40))
			return false;
		char checking = symbol & (~0x40);
		for (auto i : types) {
			if (i.symbol == checking)
				return true;
		}
		return false;
	}

	void uniseq_timing_add_value(type::i time, uniseq *out, bool addPulse) {
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

	void uniseq_timing_copy(uniseq *out, uniseq *in) {
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

		out->append(in, _addindex+1);
	}

	uniseq::uniseq(uint8_t element_size, uint16_t initial_capacity)
	{
		elsize = element_size;
		size = 0;
		capacity = 0;

		if(initial_capacity == 0){
			begin = nullptr;
			return;
		}

		reserve(initial_capacity);
	}
	uniseq::~uniseq()
	{
		U_ARRAY_FREE(begin);
		memset(this, 0, sizeof(uniseq));
	}
	void uniseq::reserve(uint16_t newcap)
	{
		if(newcap > capacity)
		{
			if(capacity)
				begin = U_ARRAY_REALLOC(begin, newcap*elsize);
			else
				begin = U_ARRAY_MALLOC(newcap*elsize);
		}
		capacity = newcap;
	}

	void uniseq::tune_capacity()
	{
		if(!capacity)
			reserve(UNICORN_CFG_uniseq_INITIAL_RESERVE);
		if(size >= capacity){
			reserve(capacity<<1);
		}
	}
	void uniseq::push_back_8(int8_t element)
	{
		tune_capacity();
		((int8_t*)begin)[size] = element;
		size++;
	}
	void uniseq::push_back_16(int16_t element)
	{
		tune_capacity();
		((int16_t*)begin)[size] = element;
		size++;
	}
	void uniseq::push_back_32(int32_t element)
	{
		tune_capacity();
		((int32_t*)begin)[size] = element;
		size++;
	}
	void uniseq::push_back_64(int64_t element)
	{
		tune_capacity();
		((int64_t*)begin)[size] = element;
		size++;
	}
	void uniseq::push_back(void* ptr)
	{
		tune_capacity();
		memcpy((void*)(((size_t)begin) + elsize * size), ptr, elsize);
		size++;
	}
	void uniseq::remove(uint16_t index)
	{
		size_t curr = (size_t)begin + elsize*index;
		size_t next = curr + elsize;
		memmove((void*)curr,(void*)next,elsize*(size-index));
		
		size--;
	}
	void uniseq::clear()
	{
		size=0;
	}
	void* uniseq::at(uint16_t index)
	{
		return (void*) ((size_t)begin + elsize*index);
	}
	void* uniseq::back()
	{
		return (void*) ((size_t)begin + elsize*(size-1));
	}
	bool uniseq::append(const uniseq* val, uint16_t from)
	{
		if(val->elsize != elsize)
			return false;
		if(val->size < from)
			return false;
		
		int16_t adding = val->size - from;
		reserve(adding + size);
		size_t ending = (size_t)begin + elsize*size;
		size_t beginning = (size_t)val->begin + elsize*from;
		memcpy((void*)ending, (void*)beginning, elsize*adding);
		
		size += adding;
		return true;
	}
}
