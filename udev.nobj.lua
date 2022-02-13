set_variable_format "%s%d"

c_module "udev" {
  --luajit_ffi = true,
  ffi_load"udev",
  include"libudev.h",

  subfiles {
    "src/error.nobj.lua",
    "src/context.nobj.lua",
    "src/device.nobj.lua",
    "src/monitor.nobj.lua",
    "src/enumerate.nobj.lua",
  },
}
