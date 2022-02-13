local function monitor_method(name, args)
  return method ("add_" .. name) { c_method_call "err_rc" ("udev_enumerate_add_" .. name) (args) }
end
local function addm (name) return function (args) return monitor_method ("match_" .. name, args) end end
local function addnom (name) return function (args) return monitor_method ("nomatch_" .. name, args) end end

object "enumerate" {
  c_source[[
    typedef struct udev_enumerate enumerate;
  ]],
  ffi_cdef[[
    typedef struct udev_enumerate enumerate;
  ]],
  destructor {
    c_method_call "void" "udev_enumerate_unref" {},
  },
  method "get_context" { c_method_call "context *" "udev_enumerate_get_udev" { } },

  addm "subsystem" { "const char *", "subsystem" },
  addnom "subsystem" { "const char *", "subsystem" },
  addm "sysattr" { "const char *", "sysattr", "const char *", "value" },
  addnom "sysattr" { "const char *", "sysattr", "const char *", "value" },
  addm "property" { "const char *", "property", "const char *", "value" },
  addm "tag" { "const char *", "tag" },
  addm "parent" { "device *", "parent" },
  addm "is_initialized" { },
  addm "sysname" { "const char *", "sysname" },

  method "add_syspath" { c_method_call "err_rc" "udev_enumerate_add_syspath" { "const char *", "syspath" } },

  method "scan_devices" { c_method_call "err_rc" "udev_enumerate_scan_devices" { } },
  method "scan_subsystems" { c_method_call "err_rc" "udev_enumerate_scan_subsystems" { } },

  method "get_list" {
    var_out { "<any>", "devices" },
    c_source [[
      lua_newtable (L);
      struct udev_list_entry *list = udev_enumerate_get_list_entry(${this}), *entry;
      int i = 1;
      udev_list_entry_foreach(entry, list) {
        const char *path = udev_list_entry_get_name(entry);
        lua_pushstring (L, path);
        lua_rawseti (L, -2, i++);
      }
    ]],
  }
}
