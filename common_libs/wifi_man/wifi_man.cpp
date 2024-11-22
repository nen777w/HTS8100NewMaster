#include "wifi_man.h"

//Network
#include <WiFi.h>
//logs
#include <dbg_msg_macro.h>


namespace devices
{

void wifi_man::connect()
{
    WiFi.setHostname(m_hostname.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(m_ap.c_str(), m_ap_pass.c_str());
    
    DBG(DBG_INFO, ("Try to connect to existing network"));

    uint8_t timeout = m_time_to_connect*2;

    // Wait for connection, 5s timeout
    do {
        delay(500);
        DBG(DBG_WIPE, ("."));
        timeout--;
    } while ( timeout && WiFi.status() != WL_CONNECTED );
    
    DBGNL(DBG_WIPE, (""));

    // not connected -> create hotspot
    if ( WiFi.status() != WL_CONNECTED ) 
    {
        DBG(DBG_INFO, ("Creating hotspot"));

        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(m_local_IP, m_local_IP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(m_hostname.c_str());

        timeout = m_time_to_connect*2;

        do {
            delay(500);
            DBG(DBG_WIPE, ("."));
            timeout--;
        } while ( timeout );

        DBGNL(DBG_WIPE, (""));
    }

    DBGNL(DBG_INFO, ("WiFi parameters:"));
    DBGNL(DBG_INFO, ("Mode: %s")(WiFi.getMode() == WIFI_AP ? "Station" : "Client"));
    if(WIFI_AP == WiFi.getMode()) 
    {
        DBGNL(DBG_INFO, ("IP address: %s")(WiFi.softAPIP().toString().c_str())); 
    } 
    else 
    {
        //WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), IPAddress(8,8,8,8));
        
        DBGNL(DBG_DEBUG, ("AP: %s, PASS: %s, DNS: %s, LocalIP: %s, GateWay: %s, Subnet: %s")
            (m_ap.c_str())
            (m_ap_pass.c_str())
            (WiFi.dnsIP().toString().c_str())
            (WiFi.localIP().toString().c_str())
            (WiFi.gatewayIP().toString().c_str())
            (WiFi.subnetMask().toString().c_str())
        );
        
        on_connect_to_internet();
    }
}

void wifi_man::disconnect()
{
    if(WIFI_AP == WiFi.getMode()) {
        WiFi.softAPdisconnect();
    } else {
        WiFi.disconnect();
    }
}

void wifi_man::start()
{
    m_internet_mode_check_last_update = m_reconnect_interval_timout_last_update = millis();
}

void wifi_man::update()
{
    if(millis() - m_reconnect_interval_timout_last_update > m_reconnect_interval_timout)
    {
        disconnect();
        connect();
        m_reconnect_interval_timout_last_update = millis();
    }

    if(millis() - m_internet_mode_check_last_update > m_internet_mode_check_timout) 
    {
        if(m_prefer_internet_mode && WIFI_AP == WiFi.getMode())
        {
            disconnect();
            connect();
        }
        m_internet_mode_check_last_update = millis();
    }
}

}
