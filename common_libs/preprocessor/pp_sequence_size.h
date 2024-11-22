#ifndef __pp_sequence_size_h__
#define __pp_sequence_size_h__

#define SEQ_SIZE(seq) SEQ_SIZE_I(seq)
#define SEQ_SIZE_I(seq) CAT(SEQ_SIZE_, SEQ_SIZE_0 seq) 

#define SEQ_SIZE_0(_) SEQ_SIZE_1
#define SEQ_SIZE_1(_) SEQ_SIZE_2
#define SEQ_SIZE_2(_) SEQ_SIZE_3
#define SEQ_SIZE_3(_) SEQ_SIZE_4 
#define SEQ_SIZE_4(_) SEQ_SIZE_5 
#define SEQ_SIZE_5(_) SEQ_SIZE_6 
#define SEQ_SIZE_6(_) SEQ_SIZE_7 
#define SEQ_SIZE_7(_) SEQ_SIZE_8 

#define SEQ_SIZE_SEQ_SIZE_0 0
#define SEQ_SIZE_SEQ_SIZE_1 1
#define SEQ_SIZE_SEQ_SIZE_2 2
#define SEQ_SIZE_SEQ_SIZE_3 3
#define SEQ_SIZE_SEQ_SIZE_4 4
#define SEQ_SIZE_SEQ_SIZE_5 5
#define SEQ_SIZE_SEQ_SIZE_6 6
#define SEQ_SIZE_SEQ_SIZE_7 7

#endif