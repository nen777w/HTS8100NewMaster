#include "timer.h"
#include <Arduino.h>

namespace tools
{
    void timer::start()
    {
        m_last_update = millis();
    }

    void timer::update()
    {
        if(millis() - m_last_update > m_interval) {
            m_cbf(m_extra);
            m_last_update = millis();
        }
    }
}