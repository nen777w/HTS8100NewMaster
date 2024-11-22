#ifndef __pp_sequence_elem_h__
#define __pp_sequence_elem_h__

#include "pp_empty.h"

#define SEQ_ELEM(i, seq) SEQ_ELEM_I(i, seq)

#define SEQ_ELEM_I(i, seq) SEQ_ELEM_II((SEQ_ELEM_ ## i seq))
#define SEQ_ELEM_II(res) SEQ_ELEM_IV(SEQ_ELEM_III res)
#define SEQ_ELEM_III(x, _) x EMPTY()
#define SEQ_ELEM_IV(x) x

#define SEQ_ELEM_0(x) x, NIL
#define SEQ_ELEM_1(_) SEQ_ELEM_0
#define SEQ_ELEM_2(_) SEQ_ELEM_1
#define SEQ_ELEM_3(_) SEQ_ELEM_2
#define SEQ_ELEM_4(_) SEQ_ELEM_3
#define SEQ_ELEM_5(_) SEQ_ELEM_4
#define SEQ_ELEM_6(_) SEQ_ELEM_5
#define SEQ_ELEM_7(_) SEQ_ELEM_6
#define SEQ_ELEM_8(_) SEQ_ELEM_7
#define SEQ_ELEM_9(_) SEQ_ELEM_8

#endif