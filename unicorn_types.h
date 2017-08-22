
#ifndef UNICORN_TYPES_H_GUARD
#define UNICORN_TYPES_H_GUARD

#include <stdint.h>
#include "unicorn_cfg.h"

#define UTYPE_q "\x1"
#define UTYPE_c "\x2"
#define UTYPE_b "\x3"
#define UTYPE_h "\x4"
#define UTYPE_i "\x5"
#define UTYPE_l "\x6"
#define UTYPE_f "\x7"
#define UTYPE_d "\x8"
#define UTYPE_n "\x9"
#define UTYPE_t "\xA"

#define UTYPE_ARR_q "\x41"
#define UTYPE_ARR_c "\x42"
#define UTYPE_ARR_b "\x43"
#define UTYPE_ARR_h "\x44"
#define UTYPE_ARR_i "\x45"
#define UTYPE_ARR_l "\x46"
#define UTYPE_ARR_f "\x47"
#define UTYPE_ARR_d "\x48"
#define UTYPE_ARR_n "\x49"
#define UTYPE_ARR_t "\x4A"

#define UTYPE_DIR_q "\x81"
#define UTYPE_DIR_c "\x82"
#define UTYPE_DIR_b "\x83"
#define UTYPE_DIR_h "\x84"
#define UTYPE_DIR_i "\x85"
#define UTYPE_DIR_l "\x86"
#define UTYPE_DIR_f "\x87"
#define UTYPE_DIR_d "\x88"
#define UTYPE_DIR_n "\x89"
#define UTYPE_DIR_t "\x8A"

#define UTYPE_DIR_ARR_q "\xC1"
#define UTYPE_DIR_ARR_c "\xC2"
#define UTYPE_DIR_ARR_b "\xC3"
#define UTYPE_DIR_ARR_h "\xC4"
#define UTYPE_DIR_ARR_i "\xC5"
#define UTYPE_DIR_ARR_l "\xC6"
#define UTYPE_DIR_ARR_f "\xC7"
#define UTYPE_DIR_ARR_d "\xC8"
#define UTYPE_DIR_ARR_n "\xC9"
#define UTYPE_DIR_ARR_t "\xCA"

#define CHAR_TYPE(x) UTYPE_##x[0]

namespace u
{
	typedef void port;

	/*
		Uniseq is the custom std::vector-like data structure
		that covers the minimum needed for unicorn.
		
		<at> is overflow-unsafe
	*/
	class uniseq{
	public:
		void* begin;
		uint16_t size, capacity;
		
		uniseq(uint8_t element_size, uint16_t initial_capacity);
		~uniseq();
		void reserve(uint16_t newcap);
		void resize(uint16_t newsize);
		
		void push_back_8(int8_t element);
		void push_back_16(int16_t element);
		void push_back_32(int32_t element);
		void push_back_64(int64_t element);
	
#if UINTPTR_MAX == 0xffff
		inline void push_back_pointer(void *element) {return push_back_16((size_t)element);};
#elif UINTPTR_MAX == 0xffffffff
		inline void push_back_pointer(void *element) {return push_back_32((size_t)element);};
#elif UINTPTR_MAX == 0xffffffffffffffff
		inline void push_back_pointer(void *element) {return push_back_64((size_t)element);};
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

	/**
		A namespace the port types are listed in.
		
		The port configuration string one-letter type identifiers
		are the same to the u::type:: identifiers.
	*/
	struct Node;  //external
	namespace type {
		typedef bool    q;  /// Boolean port
		typedef char    c;  /// ASCII symbol port

		typedef uint8_t b;  /// 8-bit integer port
		typedef int16_t h;  /// 16-bit integer port
		typedef int32_t i;  /// 32-bit integer port
		typedef int64_t l;  /// 64-bit integer port

		typedef float   f;  /// Single precision float port
		typedef double  d;  /// Double precision float port

		typedef Node n;

		typedef char t;
	}

	struct TypeDescriptor {
		char symbol;
		uint8_t size;
#if UNICORN_CFG_TYPE_DESCRIPTIONS == 1
		const char *description;
#endif
	};

	extern const TypeDescriptor types[];

	inline char get_arr_type(char symbol)
		{return symbol & 0x3F;};

	uint8_t get_type_size(char symbol);
	const char* get_type_description(char symbol);
	uint8_t get_arr_size(char symbol);
	bool is_array_type(char symbol);
};

#endif
