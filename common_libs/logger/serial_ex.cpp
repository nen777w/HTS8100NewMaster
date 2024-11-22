#include "dbg_msg_macro.h"
#include <cstdio>
#include <Arduino.h>

void serial_ex::serial_init()
{
    Serial.begin(115200); 
    while(!Serial);
}

void serial_ex::serial_printf(const char * fmt, ...)
{
    char buff[256] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    va_end(args);
    uint8_t *s = (uint8_t *)&buff;
    while (*s) Serial.write(*s++);
}