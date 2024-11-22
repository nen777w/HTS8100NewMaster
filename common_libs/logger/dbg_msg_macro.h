#ifndef __dbg_msg_macro_h__
#define __dbg_msg_macro_h__

//preprocessor
#include <pp_sequence_enum.h>
#include <pp_sequence_head.h>
#include <pp_sequence_tail.h>
#include <pp_sequence_coma_if_grathrer_than_one.h>
#include <pp_sequence_tail_enum_if_grathrer_than_one.h>
#include <serial_ex.h>

#if defined(DEBUG_MODE) && !defined(DEBUG_COMPACT_SERIAL_MODE)
//logger
#include "logger.h"

template<typename Tbase_device>
struct dbg_msg : Tbase_device
{   
    dbg_msg()
        : m_logger(template_logger::ALL)
    {}

    void init()
    {
        Tbase_device::init(m_logger.device());
    }

    template<typename... T>
    void log(template_logger::LogLevel ll, const char * message, T... args)
    {
        m_logger.log(ll, message, args...);
    }

    template<typename... T>
    void lognl(template_logger::LogLevel ll, const char * message, T... args)
    {
        m_logger.lognl(ll, message, args...);
    }

private:
    using logger_t = template_logger::logger<typename Tbase_device::device_t>;

    logger_t m_logger;
};

#endif

#if !defined(DEBUG_COMPACT_SERIAL_MODE)

#include "log_device.h"

#define DBG_INFO template_logger::INFO
#define DBG_DEBUG template_logger::DEBUG
#define DBG_WARNING template_logger::WARNING
#define DBG_ERROR template_logger::ERROR
#define DBG_CRITICAL template_logger::CRITICAL
#define DBG_WIPE template_logger::WIPE

#if defined(DEBUG_MODE)
    #define DBG_INIT() d_msg.init()
    #define DBG(ll, seq_args) d_msg.log(ll, SEQ_ENUM(seq_args))
    #define DBGNL(ll, seq_args) d_msg.lognl(ll, SEQ_ENUM(seq_args))
    #define DBGASSERT(cond) if(!cond) { DBGNL(template_logger::ASSERT, (#cond)); while(true); }
#else
    #define DBG_INIT()
    #define DBG(ll, seq_args)
    #define DBGNL(ll, seq_args)
    #define DBGASSERT(cond)
#endif

#else // defined(DEBUG_COMPACT_SERIAL_MODE)

#define DBG_INFO "INFO: "
#define DBG_DEBUG "DEBUG: "
#define DBG_WARNING "WARNING: "
#define DBG_ERROR "ERROR: "
#define DBG_CRITICAL "CRITICAL: "
#define DBG_ASSERT "ASSERT: "
#define DBG_WIPE ""


#if defined(DEBUG_MODE)
    #define DBG_INIT() serial_ex::serial_init()
#if defined(DEBUG_SERIAL_DELAY)
    #define DBG(ll, seq_args) serial_ex::serial_printf(ll SEQ_HEAD(seq_args) SEQ_COMA_IF_GRATHER_THAN_ONE(seq_args) SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE(seq_args)); delay(DEBUG_SERIAL_DELAY)
    #define DBGNL(ll, seq_args) serial_ex::serial_printf(ll SEQ_HEAD(seq_args) "\n" SEQ_COMA_IF_GRATHER_THAN_ONE(seq_args) SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE(seq_args)); delay(DEBUG_SERIAL_DELAY)
    #define DBGASSERT(cond) if(!cond) { DBGNL(DBG_ASSERT, (#cond)); while(true); }
#else            
    #define DBG(ll, seq_args) serial_ex::serial_printf(ll SEQ_HEAD(seq_args) SEQ_COMA_IF_GRATHER_THAN_ONE(seq_args) SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE(seq_args))
    #define DBGNL(ll, seq_args) serial_ex::serial_printf(ll SEQ_HEAD(seq_args) "\n" SEQ_COMA_IF_GRATHER_THAN_ONE(seq_args) SEQ_TAIL_ENUM_IF_GRATHER_THAN_ONE(seq_args))
    #define DBGASSERT(cond)
#endif    
    
#else
    #define DBG_INIT()
    #define DBG(ll, seq_args)
    #define DBGNL(ll, seq_args)
    #define DBGASSERT(cond)
#endif

#endif


#endif