#include <Arduino.h>
#include "audio_signal_detector.h"
#include "dbg_msg_macro.h"

namespace HTSNewMaster
{
    audio_signal_detector::audio_signal_detector(int analog_pin
                                               , sig_fn on_signal, sig_fn on_silience
                                               , sig_fn2 on_report_tolerance)
        : m_analog_pin(analog_pin)
        , m_on_signal(on_signal), m_on_silience(on_silience)
        , m_on_report_tolerance(on_report_tolerance)
        , m_stopped(true)
    {}

    void audio_signal_detector::start(
          unsigned long signal_timeout_ms, unsigned long silience_timeout_ms
        , uint16_t zero_level
        , uint16_t sensivity
        , uint16_t signal_tolerance_precent
        , unsigned long measure_interval_ms
    )
    {
        pinMode(m_analog_pin, INPUT);

        m_signal_timeout_ms = signal_timeout_ms; 
        m_silience_timeout_ms = silience_timeout_ms;

        m_zero_level = zero_level;
        m_sensivity = sensivity;
        m_signal_tolerance_precent = signal_tolerance_precent;

        m_measure_interval_ms = measure_interval_ms;

        DBGNL(DBG_INFO, ("(PIN: %d) signal ms: %d, silience ms: %d, measure ms: %d, sensivity: %d, tolerance: %d")
            (m_analog_pin)(m_signal_timeout_ms)(m_silience_timeout_ms)(m_measure_interval_ms)(m_sensivity)(m_signal_tolerance_precent));

        //...........
        
        m_samples_to_update_signal = m_signal_timeout_ms / m_measure_interval_ms;
        m_samples_to_update_silience = m_silience_timeout_ms / m_measure_interval_ms;

        DBGNL(DBG_INFO, ("(PIN: %d) samples to signal: %d, samples to silience: %d")
            (m_analog_pin)(m_samples_to_update_signal)(m_samples_to_update_silience));

        restart();

        DBGNL(DBG_INFO, ("Audio Detector (pin: %d) - START")(m_analog_pin));
    }

    void audio_signal_detector::silience_calibrate(unsigned long calibration_timeout, sig_fn2 calibration_report)
    {
        m_calibration_timeout = calibration_timeout;
        m_calibration_report = calibration_report;
        
        m_calibration_sample_accum = 0;
        m_calibration_samples_counter = 0;
        m_calibration_start_ms = millis();
        
        m_mode = EMode::eCalibration;
    }

    void audio_signal_detector::restart()
    {
        m_signal_counter = 0;
        m_silience_counter = 0;

        m_signal_state = EState::eUnknown;
        m_mode = EMode::eNornal;

        m_last_measure_interval_ms = millis();

        m_stopped = false;
    }

    void audio_signal_detector::stop()
    {
        m_signal_state = EState::eUnknown;
        m_stopped = true;

        DBGNL(DBG_INFO, ("Audio Detector (pin: %d) - STOP")(m_analog_pin));
    }

    void audio_signal_detector::update()
    {
        if(m_stopped) {
            return;
        }

        uint16_t sig = analogRead(m_analog_pin);
        unsigned long cur_t = millis();

        switch (m_mode)
        {
        case EMode::eNornal:
            {
                int16_t diff = sig - m_zero_level;
                uint16_t tolerance = 0;

                if(cur_t - m_last_measure_interval_ms >= m_measure_interval_ms) 
                {
                    m_last_measure_interval_ms = cur_t;

                    if(abs(diff) > m_sensivity) {
                        m_signal_counter++;
                    } else {
                        m_silience_counter++;
                    }

                    if(m_silience_counter) {
                        tolerance = (100. / (m_signal_counter + m_silience_counter)) * m_signal_counter;
                    }

                    //DBGNL(DBG_INFO, ("(PIN: %d) signal: %d, diff: %d, sig c: %d, sil c: %d, Tolerance: %d%")
                    //    (m_analog_pin)(sig)(diff)(m_signal_counter)(m_silience_counter)(tolerance));
                }

                int samples = m_signal_counter + m_silience_counter;

                if(0 != samples && 0 == (samples % m_samples_to_update_signal))
                {
                    //DBGNL(DBG_DEBUG, ("(PIN: %d) sig Tolerance: %d")(m_analog_pin)(tolerance));

                    if(EState::eSilience == m_signal_state || EState::eUnknown == m_signal_state)
                    {
                        if(tolerance >= m_signal_tolerance_precent)
                        {
                            DBGNL(DBG_DEBUG, ("(PIN: %d) sig.c: %d, sil.c: %d, sum: %d, t: %d%")
                                (m_analog_pin)(m_signal_counter)(m_silience_counter)(samples)(tolerance));

                            m_signal_state = EState::eSignal;
                            m_on_signal();
                        }
                    }
                }

                if(0 != samples && 0 == (samples % m_samples_to_update_silience))
                {
                    //DBGNL(DBG_DEBUG, ("(PIN: %d) sil Tolerance: %d")(m_analog_pin)(tolerance));

                    if(EState::eSignal == m_signal_state || EState::eUnknown == m_signal_state)
                    {
                        if(tolerance < m_signal_tolerance_precent)
                        {
                            DBGNL(DBG_DEBUG, ("(PIN: %d) sig.c: %d, sil.c: %d, sum: %d, t: %d%")
                                (m_analog_pin)(m_signal_counter)(m_silience_counter)(samples)(tolerance));

                            m_signal_state = EState::eSilience;
                            m_on_silience();
                        }
                    }
                }

                if(samples == std::max(m_samples_to_update_signal, m_samples_to_update_silience))
                {
                    DBGNL(DBG_DEBUG, ("COUNTERS RESET. (PIN: %d) sig.c: %d, sil.c: %d, sum: %d, Tolerance: %d%")
                        (m_analog_pin)(m_signal_counter)(m_silience_counter)(samples)(tolerance));

                    m_on_report_tolerance(tolerance);
                    
                    m_signal_counter = m_silience_counter = 0;
                }
            }
            break;
        case EMode::eCalibration:
            {
                if(cur_t - m_calibration_start_ms < m_calibration_timeout)
                {
                    m_calibration_sample_accum += sig;
                    m_calibration_samples_counter++;
                }
                else
                {
                    uint16_t avg = m_calibration_sample_accum / m_calibration_samples_counter;

                    DBGNL(DBG_DEBUG, ("(PIN: %d) m_calibration_sample_accum: %d, m_calibration_samples_counter: %d, Calibration: %d")
                            (m_analog_pin)(m_calibration_sample_accum)(m_calibration_samples_counter)(avg));

                    m_calibration_report(avg);
                    restart();
                }
            } 
            break;
        }
    }
}