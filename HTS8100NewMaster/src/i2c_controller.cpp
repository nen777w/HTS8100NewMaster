#include "i2c_controller.h"
#include "dbg_msg_macro.h"

namespace HTSNewMaster
{
    //ADDRESS
    const byte i2c_address = 0x7c;
    //COMMAND
    const byte cmd_volume = 0b00010000;
    const byte cmd_volume_mute_unmute = 0b00001101;
    const byte cmd_sound_mode = 0b01010010;
    
    const byte cmd_eq_bass_level = 0b00010110;
    const byte cmd_eq_treble_level = 0b00011000;
    const byte cmd_eq_mode = 0b01010001;
    const byte cmd_input_select = 0b00111111;
    const byte cmd_audio_sync = 0b01110011;
    const byte cmd_radio_preset_select = 0b10001111;
    const byte cmd_radio_full_rescan = 0b00101110;
    //DATA
    const byte data_volume_up = 0b00000001;
    const byte data_volume_down = 0b10000001;
    const byte data_volume_min = 0b00000000;
    const byte data_volume_mute = 0b00000001;
    const byte data_volume_unmute = 0b00000010;

    void i2c_controller::begin(int pin_sda, int pin_scl, int speed)
    {
        m_I2C.begin(pin_sda, pin_scl, speed);
    }

    void i2c_controller::update()
    {
        if(!m_commands.empty())
        {
            byte send_tpl[5] = {
                0b00000101
              , 0b11010100
              , 0 //COMMAND
              , 0 //VALUE
              , 0 //CRC
            };

            enum {
                CMD_POS = 2
              , VAL_POS = 3
              , CRC_POS = 4
            };

            command_info nfo = m_commands.front();
            m_commands.pop();

            using ECommand = command_info::ECommand;

            send_tpl[CMD_POS] = nfo.cmd;
            send_tpl[VAL_POS] = nfo.val;

            send_tpl[CRC_POS] = calc_crc(send_tpl);

            bool success = false;
            int nAttempt = 0;
            while(!success && nAttempt < m_send_attempts)
            {
                ++nAttempt;

                send_to_i2c(send_tpl, sizeof(send_tpl));
                byte resp[5] = {0};
                recive_from_i2c(resp, sizeof(resp));
                success = resp[CRC_POS] == calc_crc(resp);

                if(success)
                {
                    switch(nfo.command)
                    {
                        case ECommand::VOLUME_SET: //
                        case ECommand::EQ_BASS_SET: //
                        case ECommand::EQ_TREBLE_SET: //
                        {
                            //success = resp[VAL_POS] == send_tpl[VAL_POS];
                        } break;
                        case ECommand::VOLUME_MUTE: {} break;
                        case ECommand::VOLUME_UNMUTE: {} break;
                        case ECommand::SOUND_MODE_SET: {} break;
                        case ECommand::EQ_MODE_SET: {} break;
                        case ECommand::INPUT_SET: {} break;
                        case ECommand::AUDIO_SYNC_SET: {} break;
                        case ECommand::CUSTOM_COMMAND: {} break;
                    } //switch
                }
            }

            if(!success) 
            {
                DBGNL(DBG_ERROR, ("Can't send command: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x")(send_tpl[0])(send_tpl[1])(send_tpl[2])(send_tpl[3])(send_tpl[4]));
                delay(50);
            }
        }
    }

    void i2c_controller::send_to_i2c(byte *pbuf, int len)
    {
        m_I2C.beginTransmission(m_i2c_address);

        for(int n = 0; n < len; ++n)
        {
            m_I2C.write(pbuf[n]);
        }

        m_I2C.endTransmission();
    }

    void i2c_controller::recive_from_i2c(byte *pbuf, int len)
    {
        while(0 == m_I2C.requestFrom(m_i2c_address, static_cast<uint8_t>(5), true))
        {
            delay(1);
        }

        int c = 0;
        while(m_I2C.available() && c != len)
        {
            pbuf[c] = (byte)m_I2C.read();
            c++;
        }
    }

    byte i2c_controller::calc_crc(byte data[])
    {
        return static_cast<byte>(data[0] + data[1] + data[2] + data[3]);
    }

    void i2c_controller::volume_set(byte value)
    {
        add_command({command_info::ECommand::VOLUME_SET, cmd_volume, value});
    }

    void i2c_controller::volume_mute()
    {
        add_command({command_info::ECommand::VOLUME_MUTE, cmd_volume_mute_unmute, data_volume_mute});
    }

    void i2c_controller::volume_unmute()
    {
        add_command({command_info::ECommand::VOLUME_UNMUTE, cmd_volume_mute_unmute, data_volume_unmute});
    }

    void i2c_controller::sound_mode_set(eeprom_data::ESoundMode mode)
    {
        add_command({command_info::ECommand::SOUND_MODE_SET, cmd_sound_mode, static_cast<byte>(mode)});
    }

    void i2c_controller::eq_bass_set(byte value)
    {
        add_command({command_info::ECommand::EQ_BASS_SET, cmd_eq_bass_level, value});
    }

    void i2c_controller::eq_treble_set(byte value)
    {
        add_command({command_info::ECommand::EQ_TREBLE_SET, cmd_eq_treble_level, value});
    }

    void i2c_controller::eq_mode_set(eeprom_data::EEQMode mode)
    {
        add_command({command_info::ECommand::EQ_MODE_SET, cmd_eq_mode, static_cast<byte>(mode)});
    }

    void i2c_controller::input_set(eeprom_data::EInput input)
    {
        add_command({command_info::ECommand::INPUT_SET, cmd_input_select, static_cast<byte>(input)});
    }

    void i2c_controller::audio_sync_set(byte value)
    {
        add_command({command_info::ECommand::AUDIO_SYNC_SET, cmd_audio_sync, value});
    }

    void i2c_controller::radio_preset_select(byte value)
    {
        add_command({command_info::ECommand::RADIO_PRESET_SELECT, cmd_radio_preset_select, value});
    }
    
    void i2c_controller::radio_full_rescan()
    {
        add_command({command_info::ECommand::RADIO_FULL_RESCAN, cmd_radio_full_rescan, 0});
    }

    void i2c_controller::custom_commend(byte cmd, byte value)
    {
        add_command({command_info::ECommand::CUSTOM_COMMAND, cmd, value});
    }

    String i2c_controller::to_string(command_info::ECommand cmd)
    {
        using ECommand = command_info::ECommand;
        switch(cmd)
        {
            case ECommand::VOLUME_SET: return "VOLUME_SET";
            case ECommand::VOLUME_MUTE: return "VOLUME_MUTE";
            case ECommand::VOLUME_UNMUTE: return "VOLUME_UNMUTE";
            case ECommand::SOUND_MODE_SET: return "SOUND_MODE_SET";
            case ECommand::EQ_BASS_SET: return "EQ_BASS_SET";
            case ECommand::EQ_TREBLE_SET: return "EQ_TREBLE_SET";
            case ECommand::EQ_MODE_SET: return "EQ_MODE_SET";
            case ECommand::INPUT_SET: return "INPUT_SET";
            case ECommand::AUDIO_SYNC_SET: return "AUDIO_SYNC_SET";
            case ECommand::RADIO_PRESET_SELECT: return "RADIO_PRESET_SELECT";
            case ECommand::RADIO_FULL_RESCAN: return "RADIO_FULL_RESCAN";
            case ECommand::CUSTOM_COMMAND: return "CUSTOM_COMMAND";
        }

        return "Unknown";
    }

    void i2c_controller::add_command(const command_info &cmd)
    {
        DBGNL(DBG_DEBUG, ("Add command: %s, cmd: %d, data %d")(to_string(cmd.command).c_str())(cmd.cmd)(cmd.val));
        m_commands.push(cmd);
    }
}