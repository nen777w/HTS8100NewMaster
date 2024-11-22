#ifndef __log_device_h__
#define __log_device_h__

#include "dbg_msg_macro.h"

#if defined(DEBUG_MODE)
  #include "serial_log_device.h"
    
  struct base_device
  {
    using device_t = template_logger::serial_log_device;

    void init(device_t &device)
    {
        device.init(115200);
    }
  };

  using dbg_msg_t = dbg_msg<base_device>;
  extern dbg_msg_t d_msg;
#endif


#endif