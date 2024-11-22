#ifndef __DBG_MACRO_h__
#define __DBG_MACRO_h__

namespace template_logger
{
  enum LogLevel {
        WIPE = 0x40
      , INFO = 0x01|WIPE
      , DEBUG = 0x02|WIPE
      , WARNING = 0x04|WIPE
      , ERROR = 0x08|WIPE
      , CRITICAL = 0x10|WIPE
      , ASSERT = 0x20
      , ALL = INFO|DEBUG|WARNING|ERROR|CRITICAL|ASSERT
  };

  template<typename TLogDevice>
  struct logger
  {
    logger(unsigned ll = ALL)
      : m_ll(ll)
    {}

    inline TLogDevice & device() { return m_device; }

    template<typename... T>
    inline void log(LogLevel ll, const char * message, T... args)
    {
      if(is_log_enabled(ll) && m_device.is_open()) {
        log_level(ll);
        m_device.log(message, args...);
      }
    }

    template<typename... T>
    inline void lognl(LogLevel ll, const char * message, T... args)
    {
      if(is_log_enabled(ll) && m_device.is_open()) {
        log_level(ll);
        m_device.lognl(message, args...);
      }
    }

  private:
    inline bool is_log_enabled(unsigned ll) const { return m_ll & ll; }

    inline void log_level(LogLevel ll)
    {
      switch(ll)
      {
        case INFO:
        {
          m_device.log("INFO: ");
        } break;
        case DEBUG:
        {
          m_device.log("DEBUG: ");
        } break;
        case WARNING:
        {
          m_device.log("WARNING: ");
        } break;
        case ERROR:
        {
          m_device.log("ERROR: ");
        } break;
        case CRITICAL:
        {
          m_device.log("CRITICAL: ");
        } break;
        case ASSERT:
        {
          m_device.log("ASSERT: ");
        } break;
        case WIPE: break;
        default:
        {
          m_device.log("UNCNOWN(%d): ",((int)ll));
        } break;
      }
    }

  private:
    TLogDevice m_device;
    unsigned m_ll;
  };
}

#endif