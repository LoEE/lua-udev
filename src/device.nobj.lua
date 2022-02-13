function attr (name)
  return method ("get_" .. name) { c_method_call "const char *" ("udev_device_get_" .. name) { } }
end

object "device" {
  basetype "unsigned long long int" "integer",
  basetype "dev_t" "integer",
  c_source[[
    typedef struct udev_device device;
  ]],
  ffi_cdef[[
    typedef struct udev_device device;
  ]],
  destructor {
    c_method_call "void" "udev_device_unref" {},
  },

  method "__tostring" {
    var_out { "<any>", "str" },
    c_source [[
      lua_pushfstring (L, "device: %s (%s, %d)", 
        udev_device_get_devpath (${this}),
        udev_device_get_subsystem (${this}),
        udev_device_get_devnum (${this}));
    ]]
  },

  method "get_context" { c_method_call "context *" "udev_device_get_udev" { } },
  method "get_parent" { c_method_call "device *" "udev_device_get_parent" { } },
  method "get_parent_with_subsystem_devtype" {
    c_method_call "device *" "udev_device_get_parent_with_subsystem_devtype" { "const char *", "subsystem", "const char *", "?devtype" }
  },
  --[[
struct udev_device * udev_device_get_parent             (struct udev_device *udev_device);
struct udev_device * udev_device_get_parent_with_subsystem_devtype
                                                        (struct udev_device *udev_device,
                                                         const char *subsystem,
                                                         const char *devtype);
  --]]
  attr "devpath",
  attr "subsystem",
  attr "devtype",
  attr "syspath",
  attr "sysname",
  attr "sysnum",
  attr "devnode",
  method "get_is_initialized" { c_method_call "int" "udev_device_get_is_initialized" { } },
  --[[
struct udev_list_entry * udev_device_get_devlinks_list_entry
                                                        (struct udev_device *udev_device);
struct udev_list_entry * udev_device_get_properties_list_entry
                                                        (struct udev_device *udev_device);
struct udev_list_entry * udev_device_get_tags_list_entry
                                                        (struct udev_device *udev_device);
  --]]
  method "get_property_value" { c_method_call "const char *" "udev_device_get_property_value" { "const char *", "key" } },
  attr "driver",
  method "get_devnum" { c_method_call "dev_t" "udev_device_get_devnum" { } },
  attr "action",
  method "get_sysattr_value" { c_method_call "const char *" "udev_device_get_sysattr_value" { "const char *", "sysattr" } },
  --[[
struct udev_list_entry * udev_device_get_sysattr_list_entry
                                                        (struct udev_device *udev_device);
  --]]
  method "get_seqnum" { c_method_call "unsigned long long int" "udev_device_get_seqnum" { } },
  method "get_usec_since_initialized" { c_method_call "unsigned long long int" "udev_device_get_usec_since_initialized" { } },
  --[[
int                 udev_device_has_tag                 (struct udev_device *udev_device,
                                                         const char *tag);
  --]]
}
