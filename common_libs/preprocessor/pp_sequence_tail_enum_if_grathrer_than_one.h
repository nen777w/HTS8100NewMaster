#ifndef __pp_sequence_tail_enum_if_grathrer_than_one_h__
#define __pp_sequence_tail_enum_if_grathrer_than_one_h__

#include "pp_sequence_enum.h"
#include "pp_sequence_tail.h"
#include "pp_sequence_size.h"


#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE(seq) SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_I(SEQ_SIZE(seq), seq)
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_I(size, seq) CAT(SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_, size) (seq)

#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_1(seq) EMPTY()
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_2(seq) SEQ_ENUM(SEQ_TAIL(seq))
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_3(seq) SEQ_ENUM(SEQ_TAIL(seq))
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_4(seq) SEQ_ENUM(SEQ_TAIL(seq))
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_5(seq) SEQ_ENUM(SEQ_TAIL(seq))
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_6(seq) SEQ_ENUM(SEQ_TAIL(seq))
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_7(seq) SEQ_ENUM(SEQ_TAIL(seq))
#define SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE_8(seq) SEQ_ENUM(SEQ_TAIL(seq))

#endif