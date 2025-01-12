#ifndef __timer_h__
#define __timer_h__

namespace tools
{
    struct timer
    {
        typedef void(*timer_cb)(void*);
        timer(unsigned long interval)
            : m_interval(interval)
            , m_last_update(0)
        {
        }

        void set_callback(timer_cb cbf, void * extra)
        {
            m_cbf = cbf;
            m_extra = extra;
        }

        void start();
        void update();

        inline static unsigned long  msec_to_sec(double msec)
        {
            return msec / 1000;
        }

        inline static unsigned long  sec_to_msec(double sec)
        {
            return sec * 1000;
        }

    private:
        const unsigned long m_interval;
        unsigned long m_last_update;
        timer_cb m_cbf;
        void * m_extra;
    };
}

#endif