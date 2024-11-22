#ifndef __serial_log_device_h__
#define __serial_log_device_h__

#include <HardwareSerial.h>
#include "serial_ex.h"

namespace template_logger
{
    struct serial_log_device
    {
        void init(int speed)
        {
#if defined(ESP32)            
                Serial.begin(speed);
#else
	        serial_ex::serial_init();
#endif
        }

        inline bool is_open() const { return true; };
        
        template<typename... T>
        inline void log(const char * message, T... args)
        {
#if defined(ESP32)
                Serial.printf(message, args...);
#else
		serial_ex::serial_printf(message, args...);
#endif	
#if defined(DEBUG_SERIAL_DELAY)
            delay(DEBUG_SERIAL_DELAY);
#endif
        }

        template<typename... T>
        inline void lognl(const char * message, T... args)
        {
#if defined(ESP32)
                Serial.printf(message, args...);
#else
                serial_ex::serial_printf(message, args...);
#endif	
                Serial.print("\n");
#if defined(DEBUG_SERIAL_DELAY)
                delay(DEBUG_SERIAL_DELAY);
#endif            
        }
    };
}
#endif