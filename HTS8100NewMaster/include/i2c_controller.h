#ifndef __i2c_controller_h__
#define __i2c_controller_h__

#include <Wire.h>
#include <queue>
#include "settings.h"

namespace HTSNewMaster
{
    class i2c_controller
    {
    public:
        i2c_controller(uint16_t i2c_address)
            : m_I2C(0)
            , m_i2c_address(i2c_address)
        {}

    public:

        void begin(int pin_sda, int pin_scl, int speed);
        void update();

        void volume_set(byte value);
        void volume_mute();
        void volume_unmute();

        void sound_mode_set(eeprom_data::ESoundMode mode);

        void eq_bass_set(byte value);
        void eq_treble_set(byte value);
        void eq_mode_set(eeprom_data::EEQMode mode);

        void input_set(eeprom_data::EInput input);
        void audio_sync_set(byte value);

        void radio_preset_select(byte value);
        void radio_full_rescan();

        void custom_commend(byte cmd, byte value);

   private:
        byte calc_crc(byte data[]);
        void send_to_i2c(byte *pbuf, int len);
        void recive_from_i2c(byte *pbuf, int len);

    private:
        struct command_info
        {
            enum class ECommand 
            {
                    VOLUME_SET
                ,   VOLUME_MUTE
                ,   VOLUME_UNMUTE
                ,   SOUND_MODE_SET
                ,   EQ_BASS_SET
                ,   EQ_TREBLE_SET
                ,   EQ_MODE_SET
                ,   INPUT_SET
                ,   AUDIO_SYNC_SET
                ,   RADIO_PRESET_SELECT
                ,   RADIO_FULL_RESCAN
                ,   CUSTOM_COMMAND
            };

            ECommand command;
            byte cmd;
            byte val;

            command_info(ECommand _command, byte _cmd, byte _val)
                : command(_command)
                , cmd(_cmd)
                , val(_val)
            {}
        };

        std::queue<command_info> m_commands;

    private:
        void add_command(const command_info &cmd);
        String to_string(command_info::ECommand cmd);

    private:
        TwoWire m_I2C;
        const uint16_t m_i2c_address;
        const byte m_send_attempts = 3;
    };
}

#endif