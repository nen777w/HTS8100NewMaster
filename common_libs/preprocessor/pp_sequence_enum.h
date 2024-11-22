#ifndef __pp_seq_enum_h__
#define __pp_seq_enum_h__

#include "pp_cat.h"
#include "pp_sequence_size.h"

#define SEQ_ENUM(seq) SEQ_ENUM_I(SEQ_SIZE(seq), seq)
#define SEQ_ENUM_I(size, seq) CAT(SEQ_ENUM_, size) seq 

#define SEQ_ENUM_1(x) x
#define SEQ_ENUM_2(x) x, SEQ_ENUM_1
#define SEQ_ENUM_3(x) x, SEQ_ENUM_2
#define SEQ_ENUM_4(x) x, SEQ_ENUM_3
#define SEQ_ENUM_5(x) x, SEQ_ENUM_4
#define SEQ_ENUM_6(x) x, SEQ_ENUM_5
#define SEQ_ENUM_7(x) x, SEQ_ENUM_6
#define SEQ_ENUM_8(x) x, SEQ_ENUM_7

#endif