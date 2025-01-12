#ifndef __audio_signal_detector_h__
#define __audio_signal_detector_h__

#include <functional>

namespace HTSNewMaster
{
    class audio_signal_detector
    {
    public:
        using sig_fn = std::function<void()>;
        using sig_fn2 = std::function<void(uint16_t)>;

    public:
        audio_signal_detector(int analog_pin
                            , sig_fn on_signal, sig_fn on_silience
                            , sig_fn2 on_report_tolerance);
    public:
        void start(unsigned long signal_timeout_ms, unsigned long silience_timeout_ms
                 , uint16_t zero_level
                 , uint16_t sensivity
                 , uint16_t signal_tolerance_precent
                 , unsigned long measure_interval_ms
        );
        void silience_calibrate(unsigned long calibration_timeout, sig_fn2 calibration_report);
        void stop();
        void update();

    private:
        void restart();

    public:
        enum class EState { 
              eUnknown
            , eSignal
            , eSilience
        };
        inline EState state() const { return m_signal_state; }

        enum class EMode {
              eNornal
            , eCalibration
        };
        inline EMode mode() const { return m_mode; }

        inline bool is_stopped() const { return m_stopped; }

    private:
        const int m_analog_pin;
        unsigned long m_signal_timeout_ms, m_silience_timeout_ms;
        uint16_t m_zero_level;
        uint16_t m_sensivity;
        uint16_t m_signal_tolerance_precent;
        unsigned long m_measure_interval_ms;
        const sig_fn m_on_signal, m_on_silience;
        const sig_fn2 m_on_report_tolerance;

    private:
        bool m_stopped;
        EState m_signal_state;
        EMode m_mode;

    private:
        unsigned long m_calibration_timeout;
        sig_fn2 m_calibration_report;
        uint32_t m_calibration_sample_accum;
        unsigned long m_calibration_start_ms;
        unsigned long m_calibration_samples_counter;

        
    private:
        unsigned long m_samples_to_update_signal, m_samples_to_update_silience;
        unsigned long m_last_measure_interval_ms;

        int m_signal_counter;
        int m_silience_counter;
    };
}

#endif