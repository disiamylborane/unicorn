
#ifndef UNICORN_TYPES_H_GUARD
#define UNICORN_TYPES_H_GUARD

#include <stdint.h>
#include <vector>
#include "unicorn_cfg.h"

namespace u
{
	typedef void port;

	typedef uint32_t unidata;
	typedef std::vector<unidata> uniseq;
	void uniseq_add_value(unidata time, uniseq *out, bool addPulse);
	void uniseq_copy(uniseq *out, uniseq *in);

	namespace type {
		typedef bool    q;
		typedef char    c;
		typedef uint8_t b;

		typedef int16_t h;
		typedef int32_t i;
		typedef int64_t l;
		typedef float   f;
		typedef double  d;

		typedef uniseq* u;
		typedef char*   s;
	}

	struct TypeDescriptor {
		char symbol;
		uint8_t size;
#if UNICORN_CFG_TYPE_DESCRIPTIONS == 1
		const char *description;
#endif
	};

	extern TypeDescriptor types[8];

	uint8_t get_type_size(char symbol);
	bool is_array_type(char symbol);
	void* new_array_type(char symbol);
	void delete_array_type(char symbol, void* arr);
};

#endif
