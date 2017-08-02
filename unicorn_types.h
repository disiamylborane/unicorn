
#ifndef UNICORN_TYPES_H_GUARD
#define UNICORN_TYPES_H_GUARD

#include <stdint.h>
#include <vector>
#include "unicorn_cfg.h"
//#include "unicorn_block.h"

namespace u
{
	typedef void port;


	class uniseq{
	public:
		void* begin;
		uint16_t size, capacity;
		
		uniseq(uint8_t element_size, uint16_t initial_capacity);
		~uniseq();
		void reserve(uint16_t newcap);
		
		void push_back_8(int8_t element);
		void push_back_16(int16_t element);
		void push_back_32(int32_t element);
		void push_back_64(int64_t element);
	
#if UINTPTR_MAX == 0xffff
		inline void push_back_pointer(int16_t element) {return push_back_16(element);};
#elif UINTPTR_MAX == 0xffffffff
		inline void push_back_pointer(int32_t element) {return push_back_32(element);};
#elif UINTPTR_MAX == 0xffffffffffffffff
		inline void push_back_pointer(int64_t element) {return push_back_64(element);};
#else
#error "UINTPTR_MAX error: pointers are not 16, 32 or 64 bit"
#endif
		void push_back(void* ptr);
		void remove(uint16_t index);
		void clear();
		bool append(const uniseq* val, uint16_t from);
		
		void* at(uint16_t index);
		void* back();
		
	protected:
		uint8_t elsize;
		
		void tune_capacity();
	};




	namespace type {
		typedef bool    q;
		typedef char    c;
		typedef uint8_t b;

		typedef int16_t h;
		typedef int32_t i;
		typedef int64_t l;
		typedef float   f;
		typedef double  d;
		
		//Declared in block.h
		//typedef struct Node n;
	}

	struct TypeDescriptor {
		char symbol;
		uint8_t size;
#if UNICORN_CFG_TYPE_DESCRIPTIONS == 1
		const char *description;
#endif
	};
	
	void uniseq_timing_add_value(type::i time, uniseq *out, bool addPulse);
	void uniseq_timing_copy(uniseq *out, uniseq *in);

	extern TypeDescriptor types[];

	uint8_t get_type_size(char symbol);
	uint8_t get_arr_size(char symbol);
	bool is_array_type(char symbol);
};

#endif
