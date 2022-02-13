object "monitor" {
  c_source[[
    typedef struct udev_monitor monitor;
  ]],
  ffi_cdef[[
    typedef struct udev_monitor monitor;
  ]],
  destructor {
    c_method_call "void" "udev_monitor_unref" {},
  },
  method "get_context" { c_method_call "context *" "udev_monitor_get_udev" { } },
  method "enable_receiving" { c_method_call "err_rc" "udev_monitor_enable_receiving" { } },
  --[[
int                 udev_monitor_set_receive_buffer_size
                                                        (struct udev_monitor *udev_monitor,
                                                         int size);
  --]]
  method "getfd" { c_method_call "int" "udev_monitor_get_fd" { } },
  method "receive_device" { c_method_call "!device *" "udev_monitor_receive_device" { } },
  method "filter_add_match_subsystem_devtype" {
    c_method_call "err_rc" "udev_monitor_filter_add_match_subsystem_devtype" { "const char *", "subsystem", "const char *", "?devtype" }
  },
  method "filter_add_match_tag" { c_method_call "err_rc" "udev_monitor_filter_add_match_tag" { "const char *", "tag" } },
  method "filter_update" { c_method_call "err_rc" "udev_monitor_filter_update" { } },
  method "filter_remove" { c_method_call "err_rc" "udev_monitor_filter_remove" { } },
}
