meta_object "Errors" {
  map_constants_bidirectional = true,
  export_definitions {
    "EINVAL",
    "ENOMEM",
  },
  method "description" {
    var_in{ "<any>", "err" },
    var_out{ "const char *", "msg" },
    c_source "pre" [[
      int err_type;
      int err_num = -1;
    ]],
    c_source[[
      err_type = lua_type(L, ${err::idx});
      if(err_type == LUA_TSTRING) {
        lua_pushvalue(L, ${err::idx});
        lua_rawget(L, ${this::idx});
        if(lua_isnumber(L, -1)) {
          err_num = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
      } else if(err_type == LUA_TNUMBER) {
        err_num = lua_tointeger(L, ${err::idx});
      } else {
        return luaL_argerror(L, ${err::idx}, "expected string/number");
      }
      if(err_num < 0) {
        lua_pushnil(L);
        lua_pushliteral(L, "UNKNOWN ERROR");
        return 2;
      }
      ${msg} = strerror(err_num);
    ]],
  },
}

ffi_source "ffi_src" [[
-- get Errors table to map errno to error name.
local Error_names = _M.Errors
]]

c_source "extra_code" [[
static char udev_Errors_key[] = "udev_Errors_key";
]]

c_source "module_init_src" [[
	/* Cache reference to epoll.Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, udev_Errors_key);
	lua_getfield(L, -2, "Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);
]]

error_code "err_rc" "int" {
  ffi_type = "int",
  is_error_check = function(rec) return "(${" .. rec.name .. "} < 0)" end,
  ffi_is_error_check = function(rec) return "(${" .. rec.name .. "} < 0)" end,
  default = "0",
  c_source [[
    if(err < 0) {
      /* get Errors table. */
      lua_pushlightuserdata(L, udev_Errors_key);
      lua_rawget(L, LUA_REGISTRYINDEX);
      /* convert errno to string. */
      lua_rawgeti(L, -1, -err);
      /* remove Errors table. */
      lua_remove(L, -2);
      if(!lua_isnil(L, -1)) {
        /* found error. */
        return;
      }
      /* Unknown error. */
      lua_pop(L, 1);
      lua_pushnumber (L, -err);
      return;
    }
  ]],
  ffi_source [[
    if(err < 0) then
      err_str = Error_names[-err]
    end
  ]],
}
