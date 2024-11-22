#ifndef __pp_sequence_coma_if_grathrer_than_one_h__
#define __pp_sequence_coma_if_grathrer_than_one_h__

#include "pp_empty.h"
#include "pp_sequence_size.h"

#define SEQ_COMA_IF_GRATHER_THAN_ONE(seq) SEQ_COMA_IF_GRATHER_THAN_ONE_I(SEQ_SIZE(seq))
#define SEQ_COMA_IF_GRATHER_THAN_ONE_I(size) CAT(SEQ_COMA_IF_GRATHER_THAN_ONE_, size)

#define SEQ_COMA_IF_GRATHER_THAN_ONE_1 EMPTY()
#define SEQ_COMA_IF_GRATHER_THAN_ONE_2 ,
#define SEQ_COMA_IF_GRATHER_THAN_ONE_3 ,
#define SEQ_COMA_IF_GRATHER_THAN_ONE_4 ,
#define SEQ_COMA_IF_GRATHER_THAN_ONE_5 ,
#define SEQ_COMA_IF_GRATHER_THAN_ONE_6 ,
#define SEQ_COMA_IF_GRATHER_THAN_ONE_7 ,
#define SEQ_COMA_IF_GRATHER_THAN_ONE_8 ,

#endif