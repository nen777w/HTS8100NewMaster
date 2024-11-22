#ifndef __tee_log_device_h__
#define __tee_log_device_h__

namespace template_logger
{
    template<typename TLDev, typename TRDev>
    struct tee_log_device
    {
        inline TLDev & left() { return m_devL; }
        inline TRDev & right() { return m_devR; }

        inline bool is_open() const { return m_devL.is_open() || m_devR.is_open(); };
        
        template<typename... T>
        inline void log(const char * message, T... args)
        {
            if(m_devL.is_open()) {
                m_devL.log(message, args...);
            }
            if(m_devR.is_open()) {
                m_devR.log(message, args...);
            }
        }

        template<typename... T>
        inline void lognl(const char * message, T... args)
        {
            if(m_devL.is_open()) {
                m_devL.lognl(message, args...);
            }
            if(m_devR.is_open()) {
                m_devR.lognl(message, args...);
            }
        }

    private:
        TLDev m_devL;
        TRDev m_devR;
    };
}


#endif