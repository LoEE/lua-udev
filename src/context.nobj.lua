object "context" {
  c_source[[
    typedef struct udev context;
  ]],
  ffi_cdef[[
    typedef struct udev context;
  ]],
  constructor {
    c_call "context *" "udev_new" { }
  },
  destructor {
    c_method_call "void" "udev_unref" {},
  },

  method "get_log_priority" { c_method_call "int" "udev_get_log_priority" { } },
  method "set_log_priority" { c_method_call "void" "udev_set_log_priority" { "int", "priority" } },

  method "get_dev_path" { c_method_call "const char *" "udev_get_dev_path" { } },
  method "get_sys_path" { c_method_call "const char *" "udev_get_sys_path" { } },
  method "get_run_path" { c_method_call "const char *" "udev_get_run_path" { } },

  method "device_from_syspath" {
    c_method_call "!device *" "udev_device_new_from_syspath" { "const char *", "syspath" },
  },
  method "device_from_devnum" {
    c_method_call "!device *" "udev_device_new_from_devnum" { "char", "type", "dev_t", "devnum" },
  },
  method "device_from_subsystem_sysname" {
    c_method_call "!device *" "udev_device_new_from_subsystem_sysname" { "const char *", "subsystem", "const char *", "sysname" },
  },
  method "device_from_environment" {
    c_method_call "!device *" "udev_device_new_from_environment" { },
  },

  method "monitor" {
    var_in { "const char *", "name?", default = "udev" },
    c_method_call "!monitor *" "udev_monitor_new_from_netlink" { "const char *", "name" }
  },
  method "enumerate" { c_method_call "!enumerate *" "udev_enumerate_new" { } }
}
