#ifndef __sd_log_device_h__
#define __sd_log_device_h__

#include <SD.h>

namespace template_logger
{
    struct sd_log_device
    {
        ~sd_log_device() {
            m_log_file.close();
        }

        inline bool init(int cs_pin, const char * file) 
        {
            if(SD.begin(cs_pin)) {
                if(!SD.exists(file)) {
                    m_log_file = SD.open(file, FILE_WRITE);
                } else {
                    m_log_file = SD.open(file, FILE_APPEND);
                }
            }
            return is_open();
        }

        inline bool is_open() const { return m_log_file; };
        
        template<typename... T>
        inline void log(const char * message, T... args)
        {
            m_log_file.printf(message, args...);
            m_log_file.flush();
        }

        template<typename... T>
        inline void lognl(const char * message, T... args)
        {
            m_log_file.printf(message, args...);
            m_log_file.print("\n");
            m_log_file.flush();
        }

    private:
        File m_log_file;
    };
}

#endif