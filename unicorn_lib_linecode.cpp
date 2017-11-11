
#include "unicorn_library.h"
#include "unicorn_macro.h"


namespace u {
	void uniseq_linecode_add_value(type::i time, uniseq *out, bool addPulse)
	{
		if (time == 0)
			return;

		type::i _len = out->size;

		//TODO: check if optimized [byte _nowPulse = (_len % 2);]
		bool _nowPulse = !!(_len % 2);

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
	void uniseq_linecode_copy(uniseq *out, uniseq *in)
	{
		int _len = in->size;
		if (_len == 0)
			return;

		int _addindex = 0;
		if (((type::i*)in->begin)[0] == 0) {
			if (_len == 1)
				return;
			_addindex = 1;
		}
		uniseq_linecode_add_value(((type::i*)in->begin)[_addindex], out, !_addindex);

		out->append(in, _addindex + 1);
	}

	enum MatchResult {mr_false = false, mr_true = true, mr_broken};
	static const float eps = 0.07;

	bool eq(int x1, int x2) {
		int delta = x1 - x2;
		if (delta < 0)
			delta = -delta;
		return ((float)delta) / x1 < eps;
	}

	MatchResult uniseq_match_val_unsafe(uniseq *timing, int val, int *index, int* cutted)
	{
		if (*index >= timing->size)
			return mr_broken;
		int curr = ((int*)timing->begin)[*index] - *cutted;

		if (eq(val, curr)) {
			(*index)++;
			*cutted = 0;
		}
		else if (curr > val)
			*cutted += val;
		else
			return mr_false;
		return mr_true;
	}

	MatchResult uniseq_match_pulse_unsafe(uniseq* timing, int val, int* index, int* cutted)
	{
		if (val == 0)
			return mr_true;
		else if (*index % 2)
			return mr_false;
		else
			return uniseq_match_val_unsafe(timing, val, index, cutted);
	}

	MatchResult uniseq_match_space_unsafe(uniseq* timing, int val, int* index, int* cutted){
		if (val == 0)
			return mr_true;
		else if (*index % 2)
			return uniseq_match_val_unsafe(timing, val, index, cutted);
		else
			return mr_false;
	}

	MatchResult uniseq_match_timing(uniseq* timing, uniseq* val, int* index, int* cutted){
		int backupi = *index;
		int backupc = *cutted;

		for (int i = 0; i < val->size; i++){
			MatchResult mres;
			if (i % 2)
				mres = uniseq_match_space_unsafe(timing, ((int*)val->begin)[i], index, cutted);
			else
				mres = uniseq_match_pulse_unsafe(timing, ((int*)val->begin)[i], index, cutted);

			if (mres == mr_true)
				continue;

			*index = backupi;
			*cutted = backupc;
			return mres;
		}
		return mr_true;
	}

	NODE_WORK(PulseCodeModulationBlock)
	{
		int32_t code = _pl_var(i, 1);
		uint8_t _bit_count = _pl_var(b, 2);
		uniseq *_sone = _pl_arr(3);
		uniseq *_szero = _pl_arr(4);
		uniseq *out = _pl_arr(5);
		out->clear();

		for (int mask = 1 << (_bit_count - 1); mask != 0; mask >>= 1) {
			if (code & mask)
				uniseq_linecode_copy(out, _sone);
			else
				uniseq_linecode_copy(out, _szero);
		}
		_pl_callnext(0);
	}

	NODE_WORK(PulseCountModulationBlock)
	{
		int32_t code = _pl_var(i, 1);
		uniseq *out = _pl_arr(3);
		out->clear();

		for (; code > 0; code--) {
			uniseq_linecode_copy(out, _pl_arr(2));
		}
		_pl_callnext(0);
	}

	NODE_WORK(PulseLengthModulationBlock)
	{
		int32_t code = _pl_var(i, 1);
		int32_t reference = _pl_var(i, 2);
		bool spacefirst = _pl_var(q, 3);
		bool significantfirst = _pl_var(q, 4);
		bool constsyncro = _pl_var(q, 5);
		uniseq *out = _pl_arr(6);

		int32_t nonsignificant = constsyncro ? reference : (code - reference);

		for (; code > 0; code--) {
			uniseq_linecode_add_value(significantfirst ? code : nonsignificant, out, spacefirst ? false : true);
			uniseq_linecode_add_value(significantfirst ? nonsignificant : code, out, spacefirst ? true : false);
		}
		_pl_callnext(0);
	}

	NODE_WORK(PulseCodeDemodulationBlock) {
		//ref_(n) in(ARR_i, signal) in(ARR_i,one) in(ARR_i,zero) out(i,code) in(b,bitcnt) out(b,bitcnt) out(q,valid) out(q,broken)
		uniseq *in = _pl_arr(1);
		uniseq *one = _pl_arr(2);
		uniseq *zero = _pl_arr(3);

		int index = 0;
		int cutted = 0;
		int ret = 0;
		int bitcount = 0;

		int theor_bitcount = _pl_var(b,5);

		const int size = in->size;

		_pl_var(q, 8) = false;
		while ((index != size) || theor_bitcount) {
			theor_bitcount--;

			bool broken = false;
			MatchResult mres = uniseq_match_timing(in, one, &index, &cutted);
			if (mres == mr_broken)
				broken = true;
			else if (mres == mr_true) {
				ret <<= 1;
				ret |= 1;
				bitcount++;
				continue;
			}
			mres = uniseq_match_timing(in, zero, &index, &cutted);
			if (mres == mr_true) {
				ret <<= 1;
				bitcount++;
			}
			else if ((mres == mr_broken) || broken) {
				_pl_var(q, 8) = true;
				break;
			}
			else {
				_pl_var(q, 7) = false;
				_pl_callnext(0);
			}
		}
		_pl_var(q, 7) = true;
		_pl_var(i, 4) = ret;
		_pl_var(b, 6) = bitcount;

		_pl_callnext(0);
	}


	const Block lib_linecode_desc[] = {
	LIB_BLOCK_NOTUNE(PulseCodeModulationBlock,"PCM", "Pulse code modulation",
		ref_(n) in(i,code) in(b,bitcount) in(ARR_i,one) in(ARR_i,zero) out(ARR_i, out))
	LIB_BLOCK_NOTUNE(PulseCountModulationBlock, "P#M", "Pulse count modulation",
		ref_(n) in(i, code) in(ARR_i, seq) out(ARR_i, out))
	LIB_BLOCK_NOTUNE(PulseLengthModulationBlock, "PLM", "Pulse length modulation",
		ref_(n) in(i,code) in(i,reference) in(q,space first) in(q,significant first) in(q,const syncro) out(ARR_i,out))

	LIB_BLOCK_NOTUNE(PulseCodeDemodulationBlock, "PCD", "Pulse code demodulation",
		ref_(n) in(ARR_i, signal) in(ARR_i,one) in(ARR_i,zero) out(i,code) in(b,bitcnt) out(b,bitcnt) out(q,valid) out(q,broken))
	};

	extern const Library lib_linecode = { "Linecode",
		sizeof(lib_linecode_desc) / sizeof(lib_linecode_desc[0]), lib_linecode_desc };
}
