#ifndef __wifi_man_h__
#define __wifi_man_h__

#include <IPAddress.h>
//std
#include <functional>

namespace devices
{
    class wifi_man
    {
    public:
        wifi_man()
            : m_internet_mode_check_last_update(0)
            , m_reconnect_interval_timout_last_update(0)
        {}

        wifi_man( IPAddress local_IP
                , unsigned char time_to_connect
                , String hostname
                , String ap
                , String ap_pass
                , unsigned int internet_mode_check_timout
                , unsigned int reconnect_interval_timout
        ) : wifi_man()
        {
            m_local_IP = local_IP;
            m_time_to_connect = time_to_connect;
            m_prefer_internet_mode = false;
            m_internet_mode_check_timout = internet_mode_check_timout;
            m_reconnect_interval_timout = reconnect_interval_timout;

            setup(hostname, ap, ap_pass);
        }

    public:
        void setup(String hostname
                , String ap
                , String ap_pass)
        {
            
            m_hostname = hostname;
            m_ap = ap;
            m_ap_pass = ap_pass;
        }

        IPAddress apIP() const { return m_local_IP; }
        
        void connect();
        void disconnect();

        void prefer_internet_mode(bool b) { m_prefer_internet_mode = b; }

    public:
        std::function<void()> on_connect_to_internet;

    public:
        void start();
        void update();

    private:
        IPAddress m_local_IP;
        unsigned char m_time_to_connect;
        String m_hostname, m_ap, m_ap_pass;
        bool m_prefer_internet_mode;
        unsigned int m_internet_mode_check_timout, m_internet_mode_check_last_update;
        unsigned int m_reconnect_interval_timout, m_reconnect_interval_timout_last_update;
    };
}

#endif