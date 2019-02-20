#include "gen_exported.h"

namespace gen_exported {

/*******************************************************************************************************************
Cycling '74 License for Max-Generated Code for Export
Copyright (c) 2016 Cycling '74
The code that Max generates automatically and that end users are capable of exporting and using, and any
  associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
  and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
  copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
  Software must contact the copyright owner to determine if a license for commercial use is available, and the
  terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
  to licensing (at) cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
  upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
  other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
  materials or a use that generates or is intended to generate income, revenue, sales or profit.
The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*******************************************************************************************************************/

// global noise generator
Noise noise;
static const int GENLIB_LOOPCOUNT_BAIL = 100000;


// The State struct contains all the state and procedures for the gendsp kernel
typedef struct State {
	CommonState __commonstate;
	int __exception;
	int vectorsize;
	t_sample samplerate;
	t_sample m_phase_1;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_phase_1 = ((int)0);
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		t_sample * __out1 = __outs[0];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__out1 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		int source_dim = source.dim;
		int source_channels = source.channels;
		int dim_1 = source_dim;
		int window_dim = window.dim;
		int window_channels = window.channels;
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			t_sample add_2 = (m_phase_1 + in1);
			t_sample wrap_3 = wrap(add_2, ((int)0), dim_1);
			int trunc_121 = trunc(wrap_3);
			int index_clamp_2 = ((trunc_121 < 0) ? 0 : ((trunc_121 > source_dim) ? source_dim : trunc_121));
			// samples source channel 1;
			t_sample sample_source_29 = source.read(index_clamp_2, 0);
			t_sample index_source_30 = trunc_121;
			int sub_52 = (trunc_121 - ((int)4));
			int index_clamp_3 = ((sub_52 < 0) ? 0 : ((sub_52 > source_dim) ? source_dim : sub_52));
			// samples source channel 1;
			t_sample sample_source_57 = source.read(index_clamp_3, 0);
			t_sample index_source_58 = sub_52;
			int add_107 = (trunc_121 + ((int)1));
			int index_clamp_4 = ((add_107 < 0) ? 0 : ((add_107 > source_dim) ? source_dim : add_107));
			// samples source channel 1;
			t_sample sample_source_112 = source.read(index_clamp_4, 0);
			t_sample index_source_113 = add_107;
			int add_100 = (trunc_121 + ((int)2));
			int index_clamp_5 = ((add_100 < 0) ? 0 : ((add_100 > source_dim) ? source_dim : add_100));
			// samples source channel 1;
			t_sample sample_source_105 = source.read(index_clamp_5, 0);
			t_sample index_source_106 = add_100;
			int add_86 = (trunc_121 + ((int)4));
			int index_clamp_6 = ((add_86 < 0) ? 0 : ((add_86 > source_dim) ? source_dim : add_86));
			// samples source channel 1;
			t_sample sample_source_91 = source.read(index_clamp_6, 0);
			t_sample index_source_92 = add_86;
			int add_93 = (trunc_121 + ((int)3));
			int index_clamp_7 = ((add_93 < 0) ? 0 : ((add_93 > source_dim) ? source_dim : add_93));
			// samples source channel 1;
			t_sample sample_source_98 = source.read(index_clamp_7, 0);
			t_sample index_source_99 = add_93;
			int add_59 = (trunc_121 + ((int)8));
			int index_clamp_8 = ((add_59 < 0) ? 0 : ((add_59 > source_dim) ? source_dim : add_59));
			// samples source channel 1;
			t_sample sample_source_63 = source.read(index_clamp_8, 0);
			t_sample index_source_64 = add_59;
			int add_65 = (trunc_121 + ((int)7));
			int index_clamp_9 = ((add_65 < 0) ? 0 : ((add_65 > source_dim) ? source_dim : add_65));
			// samples source channel 1;
			t_sample sample_source_70 = source.read(index_clamp_9, 0);
			t_sample index_source_71 = add_65;
			int add_72 = (trunc_121 + ((int)6));
			int index_clamp_10 = ((add_72 < 0) ? 0 : ((add_72 > source_dim) ? source_dim : add_72));
			// samples source channel 1;
			t_sample sample_source_77 = source.read(index_clamp_10, 0);
			t_sample index_source_78 = add_72;
			int add_79 = (trunc_121 + ((int)5));
			int index_clamp_11 = ((add_79 < 0) ? 0 : ((add_79 > source_dim) ? source_dim : add_79));
			// samples source channel 1;
			t_sample sample_source_84 = source.read(index_clamp_11, 0);
			t_sample index_source_85 = add_79;
			int sub_4 = (trunc_121 - ((int)3));
			int index_clamp_12 = ((sub_4 < 0) ? 0 : ((sub_4 > source_dim) ? source_dim : sub_4));
			// samples source channel 1;
			t_sample sample_source_9 = source.read(index_clamp_12, 0);
			t_sample index_source_10 = sub_4;
			int sub_11 = (trunc_121 - ((int)2));
			int index_clamp_13 = ((sub_11 < 0) ? 0 : ((sub_11 > source_dim) ? source_dim : sub_11));
			// samples source channel 1;
			t_sample sample_source_16 = source.read(index_clamp_13, 0);
			t_sample index_source_17 = sub_11;
			int sub_18 = (trunc_121 - ((int)1));
			int index_clamp_14 = ((sub_18 < 0) ? 0 : ((sub_18 > source_dim) ? source_dim : sub_18));
			// samples source channel 1;
			t_sample sample_source_23 = source.read(index_clamp_14, 0);
			t_sample index_source_24 = sub_18;
			int sub_31 = (trunc_121 - ((int)7));
			int index_clamp_15 = ((sub_31 < 0) ? 0 : ((sub_31 > source_dim) ? source_dim : sub_31));
			// samples source channel 1;
			t_sample sample_source_36 = source.read(index_clamp_15, 0);
			t_sample index_source_37 = sub_31;
			int sub_38 = (trunc_121 - ((int)6));
			int index_clamp_16 = ((sub_38 < 0) ? 0 : ((sub_38 > source_dim) ? source_dim : sub_38));
			// samples source channel 1;
			t_sample sample_source_43 = source.read(index_clamp_16, 0);
			t_sample index_source_44 = sub_38;
			int sub_45 = (trunc_121 - ((int)5));
			int index_clamp_17 = ((sub_45 < 0) ? 0 : ((sub_45 > source_dim) ? source_dim : sub_45));
			// samples source channel 1;
			t_sample sample_source_50 = source.read(index_clamp_17, 0);
			t_sample index_source_51 = sub_45;
			t_sample fract_120 = fract(wrap_3);
			t_sample mul_115 = (fract_120 * ((int)1024));
			int trunc_114 = trunc(mul_115);
			bool index_ignore_18 = ((trunc_114 >= window_dim) || (trunc_114 < 0));
			// samples window channel 1;
			t_sample sample_window_60 = (index_ignore_18 ? 0 : window.read(trunc_114, 0));
			t_sample index_window_61 = trunc_114;
			t_sample mul_62 = (sample_source_63 * sample_window_60);
			int add_53 = (trunc_114 + ((int)12288));
			bool index_ignore_19 = ((add_53 >= window_dim) || (add_53 < 0));
			// samples window channel 1;
			t_sample sample_window_54 = (index_ignore_19 ? 0 : window.read(add_53, 0));
			t_sample index_window_55 = add_53;
			t_sample mul_56 = (sample_source_57 * sample_window_54);
			int add_108 = (trunc_114 + ((int)7168));
			bool index_ignore_20 = ((add_108 >= window_dim) || (add_108 < 0));
			// samples window channel 1;
			t_sample sample_window_109 = (index_ignore_20 ? 0 : window.read(add_108, 0));
			t_sample index_window_110 = add_108;
			t_sample mul_111 = (sample_source_112 * sample_window_109);
			int add_101 = (trunc_114 + ((int)6144));
			bool index_ignore_21 = ((add_101 >= window_dim) || (add_101 < 0));
			// samples window channel 1;
			t_sample sample_window_102 = (index_ignore_21 ? 0 : window.read(add_101, 0));
			t_sample index_window_103 = add_101;
			t_sample mul_104 = (sample_source_105 * sample_window_102);
			int add_87 = (trunc_114 + ((int)4096));
			bool index_ignore_22 = ((add_87 >= window_dim) || (add_87 < 0));
			// samples window channel 1;
			t_sample sample_window_88 = (index_ignore_22 ? 0 : window.read(add_87, 0));
			t_sample index_window_89 = add_87;
			t_sample mul_90 = (sample_source_91 * sample_window_88);
			int add_94 = (trunc_114 + ((int)5120));
			bool index_ignore_23 = ((add_94 >= window_dim) || (add_94 < 0));
			// samples window channel 1;
			t_sample sample_window_95 = (index_ignore_23 ? 0 : window.read(add_94, 0));
			t_sample index_window_96 = add_94;
			t_sample mul_97 = (sample_source_98 * sample_window_95);
			int add_66 = (trunc_114 + ((int)1024));
			bool index_ignore_24 = ((add_66 >= window_dim) || (add_66 < 0));
			// samples window channel 1;
			t_sample sample_window_67 = (index_ignore_24 ? 0 : window.read(add_66, 0));
			t_sample index_window_68 = add_66;
			t_sample mul_69 = (sample_source_70 * sample_window_67);
			int add_73 = (trunc_114 + ((int)2048));
			bool index_ignore_25 = ((add_73 >= window_dim) || (add_73 < 0));
			// samples window channel 1;
			t_sample sample_window_74 = (index_ignore_25 ? 0 : window.read(add_73, 0));
			t_sample index_window_75 = add_73;
			t_sample mul_76 = (sample_source_77 * sample_window_74);
			int add_80 = (trunc_114 + ((int)3072));
			bool index_ignore_26 = ((add_80 >= window_dim) || (add_80 < 0));
			// samples window channel 1;
			t_sample sample_window_81 = (index_ignore_26 ? 0 : window.read(add_80, 0));
			t_sample index_window_82 = add_80;
			t_sample mul_83 = (sample_source_84 * sample_window_81);
			int add_5 = (trunc_114 + ((int)11264));
			bool index_ignore_27 = ((add_5 >= window_dim) || (add_5 < 0));
			// samples window channel 1;
			t_sample sample_window_6 = (index_ignore_27 ? 0 : window.read(add_5, 0));
			t_sample index_window_7 = add_5;
			t_sample mul_8 = (sample_source_9 * sample_window_6);
			int add_12 = (trunc_114 + ((int)10240));
			bool index_ignore_28 = ((add_12 >= window_dim) || (add_12 < 0));
			// samples window channel 1;
			t_sample sample_window_13 = (index_ignore_28 ? 0 : window.read(add_12, 0));
			t_sample index_window_14 = add_12;
			t_sample mul_15 = (sample_source_16 * sample_window_13);
			int add_19 = (trunc_114 + ((int)9216));
			bool index_ignore_29 = ((add_19 >= window_dim) || (add_19 < 0));
			// samples window channel 1;
			t_sample sample_window_20 = (index_ignore_29 ? 0 : window.read(add_19, 0));
			t_sample index_window_21 = add_19;
			t_sample mul_22 = (sample_source_23 * sample_window_20);
			int add_25 = (trunc_114 + ((int)8192));
			bool index_ignore_30 = ((add_25 >= window_dim) || (add_25 < 0));
			// samples window channel 1;
			t_sample sample_window_26 = (index_ignore_30 ? 0 : window.read(add_25, 0));
			t_sample index_window_27 = add_25;
			t_sample mul_28 = (sample_source_29 * sample_window_26);
			int add_32 = (trunc_114 + ((int)15360));
			bool index_ignore_31 = ((add_32 >= window_dim) || (add_32 < 0));
			// samples window channel 1;
			t_sample sample_window_33 = (index_ignore_31 ? 0 : window.read(add_32, 0));
			t_sample index_window_34 = add_32;
			t_sample mul_35 = (sample_source_36 * sample_window_33);
			int add_39 = (trunc_114 + ((int)14336));
			bool index_ignore_32 = ((add_39 >= window_dim) || (add_39 < 0));
			// samples window channel 1;
			t_sample sample_window_40 = (index_ignore_32 ? 0 : window.read(add_39, 0));
			t_sample index_window_41 = add_39;
			t_sample mul_42 = (sample_source_43 * sample_window_40);
			int add_46 = (trunc_114 + ((int)13312));
			bool index_ignore_33 = ((add_46 >= window_dim) || (add_46 < 0));
			// samples window channel 1;
			t_sample sample_window_47 = (index_ignore_33 ? 0 : window.read(add_46, 0));
			t_sample index_window_48 = add_46;
			t_sample mul_49 = (sample_source_50 * sample_window_47);
			t_sample out1 = (((((((((((((((mul_49 + mul_42) + mul_35) + mul_28) + mul_22) + mul_15) + mul_8) + mul_83) + mul_76) + mul_69) + mul_62) + mul_97) + mul_90) + mul_104) + mul_111) + mul_56);
			t_sample phase_next_122 = fixdenorm(wrap_3);
			m_phase_1 = phase_next_122;
			// assign results to output buffer;
			(*(__out1++)) = out1;
			
		};
		return __exception;
		
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 1;
int gen_kernel_numouts = 1;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 0; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "playback rate" };
const char *gen_kernel_outnames[] = { "interpolated output" };

/// Invoke the signal process of a State object

int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n) {
	State* self = (State *)cself;
	return self->perform(ins, outs, n);
}

/// Reset all parameters and stateful operators of a State object

void reset(CommonState *cself) {
	State* self = (State *)cself;
	self->reset(cself->sr, cself->vs);
}

/// Set a parameter of a State object

void setparameter(CommonState *cself, long index, t_param value, void *ref) {
	State *self = (State *)cself;
	switch (index) {
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		
		default: break;
	}
}

/// Get the name of a parameter of a State object

const char *getparametername(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].name;
	}
	return 0;
}

/// Get the minimum value of a parameter of a State object

t_param getparametermin(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmin;
	}
	return 0;
}

/// Get the maximum value of a parameter of a State object

t_param getparametermax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmax;
	}
	return 0;
}

/// Get parameter of a State object has a minimum and maximum value

char getparameterhasminmax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].hasminmax;
	}
	return 0;
}

/// Get the units of a parameter of a State object

const char *getparameterunits(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].units;
	}
	return 0;
}

/// Get the size of the state of all parameters of a State object

size_t getstatesize(CommonState *cself) {
	return genlib_getstatesize(cself, &getparameter);
}

/// Get the state of all parameters of a State object

short getstate(CommonState *cself, char *state) {
	return genlib_getstate(cself, state, &getparameter);
}

/// set the state of all parameters of a State object

short setstate(CommonState *cself, const char *state) {
	return genlib_setstate(cself, state, &setparameter);
}

/// Allocate and configure a new State object and it's internal CommonState:

void *create(t_param sr, long vs) {
	State *self = new State;
	self->reset(sr, vs);
	ParamInfo *pi;
	self->__commonstate.inputnames = gen_kernel_innames;
	self->__commonstate.outputnames = gen_kernel_outnames;
	self->__commonstate.numins = gen_kernel_numins;
	self->__commonstate.numouts = gen_kernel_numouts;
	self->__commonstate.sr = sr;
	self->__commonstate.vs = vs;
	self->__commonstate.params = 0;
	self->__commonstate.numparams = 0;
	
	return self;
}

/// Release all resources and memory used by a State object:

void destroy(CommonState *cself) {
	State *self = (State *)cself;
	
	delete self;
}


} // gen_exported::
