/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* some Lua 5.0 compatibility support. */
#if !defined(lua_pushliteral)
#define lua_pushliteral(L, s) lua_pushstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#endif

#if !defined(LUA_VERSION_NUM)
#define lua_pushinteger(L, n) lua_pushnumber(L, (lua_Number)n)
#define luaL_Reg luaL_reg
#endif

/* some Lua 5.1 compatibility support. */
#if !defined(LUA_VERSION_NUM) || (LUA_VERSION_NUM == 501)
/*
** Adapted from Lua 5.2.0
*/
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushstring(L, l->name);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source)

#define lua_rawlen(L, idx) lua_objlen(L, idx)

#endif

#if LUA_VERSION_NUM == 502

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source, NULL)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

#endif

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_MODULES_AS_GLOBALS 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 1
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 0


#include "libudev.h"



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#include <stdbool.h>
#else

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

void *nobj_realloc(void *ptr, size_t osize, size_t nsize);

void *nobj_realloc(void *ptr, size_t osize, size_t nsize) {
	(void)osize;
	if(0 == nsize) {
		free(ptr);
		return NULL;
	}
	return realloc(ptr, nsize);
}

#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	nobj_realloc((obj), sizeof(type), 0); \
} while(0)

#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = nobj_realloc(NULL, 0, sizeof(type)); \
} while(0)

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
#define OBJ_TYPE_IMPORT (1<<2)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef enum {
	REG_OBJECT,
	REG_PACKAGE,
	REG_META,
} module_reg_type;

typedef struct reg_impl {
	const char *if_name;
	const void *impl;
} reg_impl;

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_Reg  *pub_funcs;
	const luaL_Reg  *methods;
	const luaL_Reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	const reg_impl  *implements;
	int             bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char obj_udata_weak_ref_key[] = "obj_udata_weak_ref_key";

/* use static pointer as key to module's private table. */
static char obj_udata_private_key[] = "obj_udata_private_key";

#if LUAJIT_FFI
typedef int (*ffi_export_func_t)(void);
typedef struct ffi_export_symbol {
	const char *name;
	union {
	void               *data;
	ffi_export_func_t  func;
	} sym;
} ffi_export_symbol;
#endif



typedef int err_rc;

static void error_code__err_rc__push(lua_State *L, err_rc err);


static obj_type obj_types[] = {
#define obj_type_id_Errors 0
#define obj_type_Errors (obj_types[obj_type_id_Errors])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "Errors" },
#define obj_type_id_context 1
#define obj_type_context (obj_types[obj_type_id_context])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "context" },
#define obj_type_id_device 2
#define obj_type_device (obj_types[obj_type_id_device])
  { NULL, 2, OBJ_TYPE_FLAG_WEAK_REF, "device" },
#define obj_type_id_monitor 3
#define obj_type_monitor (obj_types[obj_type_id_monitor])
  { NULL, 3, OBJ_TYPE_FLAG_WEAK_REF, "monitor" },
#define obj_type_id_enumerate 4
#define obj_type_enumerate (obj_types[obj_type_id_enumerate])
  { NULL, 4, OBJ_TYPE_FLAG_WEAK_REF, "enumerate" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI

/* nobj_ffi_support_enabled_hint should be set to 1 when FFI support is enabled in at-least one
 * instance of a LuaJIT state.  It should never be set back to 0. */
static int nobj_ffi_support_enabled_hint = 0;
static const char nobj_ffi_support_key[] = "LuaNativeObject_FFI_SUPPORT";
static const char nobj_check_ffi_support_code[] =
"local stat, ffi=pcall(require,\"ffi\")\n" /* try loading LuaJIT`s FFI module. */
"if not stat then return false end\n"
"return true\n";

static int nobj_check_ffi_support(lua_State *L) {
	int rc;
	int err;

	/* check if ffi test has already been done. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) {
		rc = lua_toboolean(L, -1);
		lua_pop(L, 1);
		/* use results of previous check. */
		goto finished;
	}
	lua_pop(L, 1); /* pop nil. */

	err = luaL_loadbuffer(L, nobj_check_ffi_support_code,
		sizeof(nobj_check_ffi_support_code) - 1, nobj_ffi_support_key);
	if(0 == err) {
		err = lua_pcall(L, 0, 1, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Error when checking for FFI-support: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
		return 0;
	}
	/* check results of test. */
	rc = lua_toboolean(L, -1);
	lua_pop(L, 1); /* pop results. */
		/* cache results. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_pushboolean(L, rc);
	lua_rawset(L, LUA_REGISTRYINDEX);

finished:
	/* turn-on hint that there is FFI code enabled. */
	if(rc) {
		nobj_ffi_support_enabled_hint = 1;
	}

	return rc;
}

typedef struct {
	const char **ffi_init_code;
	int offset;
} nobj_reader_state;

static const char *nobj_lua_Reader(lua_State *L, void *data, size_t *size) {
	nobj_reader_state *state = (nobj_reader_state *)data;
	const char *ptr;

	(void)L;
	ptr = state->ffi_init_code[state->offset];
	if(ptr != NULL) {
		*size = strlen(ptr);
		state->offset++;
	} else {
		*size = 0;
	}
	return ptr;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code[], const ffi_export_symbol *ffi_exports, int priv_table)
{
	nobj_reader_state state = { ffi_init_code, 0 };
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym.data);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = lua_load_no_mode(L, nobj_lua_Reader, &state, ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


typedef struct {
	void *impl;
	void *obj;
} obj_implement;

static FUNC_UNUSED void *obj_implement_luaoptional(lua_State *L, int _index, void **impl, char *if_name) {
	void *ud;
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	/* get the implements table for this interface. */
	lua_pushlightuserdata(L, if_name);
	lua_rawget(L, LUA_REGISTRYINDEX);

	/* get pointer to userdata value & check if it is a userdata value. */
	ud = (obj_implement *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* get the userdata's metatable */
		if(lua_getmetatable(L, _index)) {
			/* lookup metatable in interface table for this object's implementation of the interface. */
			lua_gettable(L, -2);
		} else {
			/* no metatable. */
			goto no_interface;
		}
#if LUAJIT_FFI
	} else if(nobj_ffi_support_enabled_hint) { /* handle cdata. */
		/* get cdata interface check function from interface table. */
		lua_getfield(L, -1, "cdata");
		if(lua_isfunction(L, -1)) {
			/* pass cdata to function, return value should be an implmentation. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			/* get pointer to cdata. */
			ud = (void *)lua_topointer(L, _index);
		} else {
			lua_pop(L, 1); /* pop non-function. */
			goto no_interface;
		}
#endif
	} else {
		goto no_interface;
	}

	if(!lua_isnil(L, -1)) {
		*impl = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop interface table & implementation. */
		/* object implements interface. */
		return ud;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
no_interface:
	lua_pop(L, 1); /* pop interface table. */
	return NULL;
}

static FUNC_UNUSED void *obj_implement_luacheck(lua_State *L, int _index, void **impl, char *type) {
	void *ud = obj_implement_luaoptional(L, _index, impl, type);
	if(ud == NULL) {
#define ERROR_BUFFER_SIZE 256
		char buf[ERROR_BUFFER_SIZE];
		snprintf(buf, ERROR_BUFFER_SIZE-1,"Expected object with %s interface", type);
		/* value doesn't implement this interface. */
		luaL_argerror(L, _index, buf);
	}
	return ud;
}

/* use static pointer as key to interfaces table. (version 1.0) */
static char obj_interfaces_table_key[] = "obj_interfaces<1.0>_table_key";

static void obj_get_global_interfaces_table(lua_State *L) {
	/* get global interfaces table. */
	lua_getfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	if(lua_isnil(L, -1)) {
		/* Need to create global interfaces table. */
		lua_pop(L, 1); /* pop nil */
		lua_createtable(L, 0, 4); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		/* store interfaces table in Lua registery. */
		lua_setfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	}
}

static void obj_get_interface(lua_State *L, const char *name, int global_if_tab) {
	/* get a interface's implementation table */
	lua_getfield(L, global_if_tab, name);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil */
		/* new interface. (i.e. no object implement it yet.)
		 *
		 * create an empty table for this interface that will be used when an
		 * implementation is registered for this interface.
		 */
		lua_createtable(L, 0, 2); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		lua_setfield(L, global_if_tab, name); /* store interface in global interfaces table. */
	}
}

static int obj_get_userdata_interface(lua_State *L) {
	/* get the userdata's metatable */
	if(lua_getmetatable(L, 2)) {
		/* lookup metatable in interface table for the userdata's implementation of the interface. */
		lua_gettable(L, 1);
		if(!lua_isnil(L, -1)) {
			/* return the implementation. */
			return 1;
		}
	}
	/* no metatable or no implementation. */
	return 0;
}

static void obj_interface_register(lua_State *L, char *name, int global_if_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, name, global_if_tab);

	/* check for 'userdata' function. */
	lua_getfield(L, -1, "userdata");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* add C function for getting a userdata's implementation. */
		lua_pushcfunction(L, obj_get_userdata_interface);
		lua_setfield(L, -2, "userdata");
	} else {
		/* already have function. */
		lua_pop(L, 1); /* pop C function. */
	}
	/* we are going to use a lightuserdata pointer for fast lookup of the interface's impl. table. */
	lua_pushlightuserdata(L, name);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void obj_register_interfaces(lua_State *L, char *interfaces[]) {
	int i;
	int if_tab;
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(i = 0; interfaces[i] != NULL ; i++) {
		obj_interface_register(L, interfaces[i], if_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

static void obj_type_register_implement(lua_State *L, const reg_impl *impl, int global_if_tab, int mt_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, impl->if_name, global_if_tab);

	/* register object's implement in the interface table. */
	lua_pushvalue(L, mt_tab);
	lua_pushlightuserdata(L, (void *)impl->impl);
	lua_settable(L, -3);

	lua_pop(L, 1); /* pop inteface table. */
}

static void obj_type_register_implements(lua_State *L, const reg_impl *impls) {
	int if_tab;
	int mt_tab;
	/* get absolute position of object's metatable. */
	mt_tab = lua_gettop(L);
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(; impls->if_name != NULL ; impls++) {
		obj_type_register_implement(L, impls, if_tab, mt_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

/* For Lua 5.2 don't register modules as globals. */
#if LUA_VERSION_NUM == 502
#undef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED int obj_import_external_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the metatable. */
		/* REGISTERY[lightuserdata<type>] = REGISTERY[type->name] */
		lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
		lua_pushvalue(L, -2); /* dup. type's metatable. */
		lua_rawset(L, LUA_REGISTRYINDEX); /* save external type's metatable. */
		/* NOTE: top of Lua stack still has the type's metatable. */
		return 1;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED int obj_import_external_ffi_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the C check function. */
		/* _priv_table[lightuserdata<type>] = REGISTERY[type->name].c_check */
		lua_getfield(L, -1, "c_check");
		lua_remove(L, -2); /* remove metatable. */
		if(lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
			lua_pushvalue(L, -2); /* dup. check function */
			lua_rawset(L, -4); /* save check function to module's private table. */
			/* NOTE: top of Lua stack still has the type's C check function. */
			return 1;
		} else {
			lua_pop(L, 1); /* pop non function value. */
		}
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_rawlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		if((type->flags & OBJ_TYPE_IMPORT) == 0) {
			/* can't resolve internal type. */
			luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
		}
		/* try to import external type. */
		if(obj_import_external_type(L, type)) {
			/* imported type, re-try metatable check. */
			goto recheck_metatable;
		}
		/* External type not yet available, so the object can't be compatible. */
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type, int not_delete) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					if(not_delete) {
						luaL_error(L, "null %s", type->name); /* object was garbage collected? */
					}
					return NULL;
				}
				return ud;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				*obj = *(void **)lua_topointer(L, _index);
				return ud;
			}
			lua_pop(L, 2);
		} else {
			lua_pop(L, 1); /* pop nil. */
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	if(not_delete) {
		luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	}
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	if(lua_isnoneornil(L, _index)) {
		return obj;
	}
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		lua_remove(L, -2);
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			} else if(lua_isnil(L, -1)) {
				lua_pop(L, 1); /* pop nil. */
				if((type->flags & OBJ_TYPE_IMPORT) == 0) {
					/* can't resolve internal type. */
					luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
				}
				/* try to import external type. */
				if(obj_import_external_type(L, type)) {
					/* imported type, re-try metatable check. */
					goto recheck_metatable;
				}
				/* External type not yet available, so the object can't be compatible. */
				return 0;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				return (void *)lua_topointer(L, _index);
			}
		} else {
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	void *ud;
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_call(L, 1, 1);
		return obj;
	}
#endif
	/* create new userdata. */
	ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_rawlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_rawlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx,
		int bidirectional) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		/* map values back to keys. */
		if(bidirectional) {
			/* check if value already exists. */
			lua_pushvalue(L, -1);
			lua_rawget(L, tab_idx - 3);
			if(lua_isnil(L, -1)) {
				lua_pop(L, 1);
				/* add value->key mapping. */
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -3);
				lua_rawset(L, tab_idx - 4);
			} else {
				/* value already exists. */
				lua_pop(L, 1);
			}
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	luaL_setfuncs(L, type_reg->methods, 0);

	/* create metatable table. */
	lua_newtable(L);
	luaL_setfuncs(L, type_reg->metas, 0); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_Reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->req_type == REG_PACKAGE) {
		obj_type_register_package(L, type_reg);
		return;
	}
	if(type_reg->req_type == REG_META) {
		obj_type_register_meta(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		luaL_setfuncs(L, reg_list, 0); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	luaL_setfuncs(L, type_reg->methods, 0); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */

	luaL_setfuncs(L, type_reg->metas, 0); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

	obj_type_register_implements(L, type_reg->implements);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

/* use static pointer as key to weak callback_state table. */
static char obj_callback_state_weak_ref_key[] = "obj_callback_state_weak_ref_key";

static FUNC_UNUSED void *nobj_get_callback_state(lua_State *L, int owner_idx, int size) {
	void *cb_state;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* create weak table for callback_state */
		lua_newtable(L);               /* weak table. */
		lua_newtable(L);               /* metatable for weak table. */
		lua_pushliteral(L, "__mode");
		lua_pushliteral(L, "k");
		lua_rawset(L, -3);             /* metatable.__mode = 'k'  weak keys. */
		lua_setmetatable(L, -2);       /* add metatable to weak table. */
		lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
		lua_pushvalue(L, -2);          /* dup weak table. */
		lua_rawset(L, LUA_REGISTRYINDEX);  /* add weak table to registry. */
	}

	/* check weak table for callback_state. */
	lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
		/* create new callback state. */
		cb_state = lua_newuserdata(L, size);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	} else {
		/* got existing callback state. */
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop <weak table>, <callback_state> */
	}

	return cb_state;
}

static FUNC_UNUSED void *nobj_delete_callback_state(lua_State *L, int owner_idx) {
	void *cb_state = NULL;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil.  no weak table, so there is no callback state. */
		return NULL;
	}
	/* get callback state. */
	lua_pushvalue(L, owner_idx); /* dup. owner */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2); /* pop <weak table>, nil.  No callback state for the owner. */
	} else {
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop <state> */
		/* remove callback state. */
		lua_pushvalue(L, owner_idx); /* dup. owner */
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	}

	return cb_state;
}



static char *obj_interfaces[] = {
  NULL,
};



#define obj_type_context_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_context))
#define obj_type_context_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_context))
#define obj_type_context_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_context), flags)
#define obj_type_context_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_context), flags)

#define obj_type_device_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_device))
#define obj_type_device_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_device))
#define obj_type_device_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_device), flags)
#define obj_type_device_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_device), flags)

#define obj_type_monitor_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_monitor))
#define obj_type_monitor_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_monitor))
#define obj_type_monitor_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_monitor), flags)
#define obj_type_monitor_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_monitor), flags)

#define obj_type_enumerate_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_enumerate))
#define obj_type_enumerate_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_enumerate))
#define obj_type_enumerate_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_enumerate), flags)
#define obj_type_enumerate_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_enumerate), flags)



static char udev_Errors_key[] = "udev_Errors_key";

    typedef struct udev context;
  
    typedef struct udev_device device;
  
    typedef struct udev_monitor monitor;
  
    typedef struct udev_enumerate enumerate;
  


/* method: description */
static int Errors__description__meth(lua_State *L) {
  const char * msg1 = NULL;
      int err_type;
      int err_num = -1;
    
      err_type = lua_type(L, 2);
      if(err_type == LUA_TSTRING) {
        lua_pushvalue(L, 2);
        lua_rawget(L, 1);
        if(lua_isnumber(L, -1)) {
          err_num = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
      } else if(err_type == LUA_TNUMBER) {
        err_num = lua_tointeger(L, 2);
      } else {
        return luaL_argerror(L, 2, "expected string/number");
      }
      if(err_num < 0) {
        lua_pushnil(L);
        lua_pushliteral(L, "UNKNOWN ERROR");
        return 2;
      }
      msg1 = strerror(err_num);
    
  lua_pushstring(L, msg1);
  return 1;
}

static void error_code__err_rc__push(lua_State *L, err_rc err) {
  const char *err_str = NULL;
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
  
	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: new */
static int context__new__meth(lua_State *L) {
  int this_flags1 = OBJ_UDATA_FLAG_OWN;
  context * this1;
  this1 = udev_new();
  obj_type_context_push(L, this1, this_flags1);
  return 1;
}

/* method: _priv */
static int context__delete__meth(lua_State *L) {
  int this_flags1 = 0;
  context * this1;
  this1 = obj_type_context_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  udev_unref(this1);
  return 0;
}

/* method: get_log_priority */
static int context__get_log_priority__meth(lua_State *L) {
  context * this1;
  int rc_udev_get_log_priority1 = 0;
  this1 = obj_type_context_check(L,1);
  rc_udev_get_log_priority1 = udev_get_log_priority(this1);
  lua_pushinteger(L, rc_udev_get_log_priority1);
  return 1;
}

/* method: set_log_priority */
static int context__set_log_priority__meth(lua_State *L) {
  context * this1;
  int priority2;
  this1 = obj_type_context_check(L,1);
  priority2 = luaL_checkinteger(L,2);
  udev_set_log_priority(this1, priority2);
  return 0;
}

/* method: get_dev_path */
static int context__get_dev_path__meth(lua_State *L) {
  context * this1;
  const char * rc_udev_get_dev_path1 = NULL;
  this1 = obj_type_context_check(L,1);
  rc_udev_get_dev_path1 = udev_get_dev_path(this1);
  lua_pushstring(L, rc_udev_get_dev_path1);
  return 1;
}

/* method: get_sys_path */
static int context__get_sys_path__meth(lua_State *L) {
  context * this1;
  const char * rc_udev_get_sys_path1 = NULL;
  this1 = obj_type_context_check(L,1);
  rc_udev_get_sys_path1 = udev_get_sys_path(this1);
  lua_pushstring(L, rc_udev_get_sys_path1);
  return 1;
}

/* method: get_run_path */
static int context__get_run_path__meth(lua_State *L) {
  context * this1;
  const char * rc_udev_get_run_path1 = NULL;
  this1 = obj_type_context_check(L,1);
  rc_udev_get_run_path1 = udev_get_run_path(this1);
  lua_pushstring(L, rc_udev_get_run_path1);
  return 1;
}

/* method: device_from_syspath */
static int context__device_from_syspath__meth(lua_State *L) {
  context * this1;
  size_t syspath_len2;
  const char * syspath2;
  int rc_udev_device_new_from_syspath_flags1 = OBJ_UDATA_FLAG_OWN;
  device * rc_udev_device_new_from_syspath1;
  this1 = obj_type_context_check(L,1);
  syspath2 = luaL_checklstring(L,2,&(syspath_len2));
  rc_udev_device_new_from_syspath1 = udev_device_new_from_syspath(this1, syspath2);
  obj_type_device_push(L, rc_udev_device_new_from_syspath1, rc_udev_device_new_from_syspath_flags1);
  return 1;
}

/* method: device_from_devnum */
static int context__device_from_devnum__meth(lua_State *L) {
  context * this1;
  char type2;
  dev_t devnum3;
  int rc_udev_device_new_from_devnum_flags1 = OBJ_UDATA_FLAG_OWN;
  device * rc_udev_device_new_from_devnum1;
  this1 = obj_type_context_check(L,1);
  type2 = luaL_checkinteger(L,2);
  devnum3 = luaL_checkinteger(L,3);
  rc_udev_device_new_from_devnum1 = udev_device_new_from_devnum(this1, type2, devnum3);
  obj_type_device_push(L, rc_udev_device_new_from_devnum1, rc_udev_device_new_from_devnum_flags1);
  return 1;
}

/* method: device_from_subsystem_sysname */
static int context__device_from_subsystem_sysname__meth(lua_State *L) {
  context * this1;
  size_t subsystem_len2;
  const char * subsystem2;
  size_t sysname_len3;
  const char * sysname3;
  int rc_udev_device_new_from_subsystem_sysname_flags1 = OBJ_UDATA_FLAG_OWN;
  device * rc_udev_device_new_from_subsystem_sysname1;
  this1 = obj_type_context_check(L,1);
  subsystem2 = luaL_checklstring(L,2,&(subsystem_len2));
  sysname3 = luaL_checklstring(L,3,&(sysname_len3));
  rc_udev_device_new_from_subsystem_sysname1 = udev_device_new_from_subsystem_sysname(this1, subsystem2, sysname3);
  obj_type_device_push(L, rc_udev_device_new_from_subsystem_sysname1, rc_udev_device_new_from_subsystem_sysname_flags1);
  return 1;
}

/* method: device_from_environment */
static int context__device_from_environment__meth(lua_State *L) {
  context * this1;
  int rc_udev_device_new_from_environment_flags1 = OBJ_UDATA_FLAG_OWN;
  device * rc_udev_device_new_from_environment1;
  this1 = obj_type_context_check(L,1);
  rc_udev_device_new_from_environment1 = udev_device_new_from_environment(this1);
  obj_type_device_push(L, rc_udev_device_new_from_environment1, rc_udev_device_new_from_environment_flags1);
  return 1;
}

/* method: monitor */
static int context__monitor__meth(lua_State *L) {
  context * this1;
  size_t name_len2;
  const char * name2;
  int rc_udev_monitor_new_from_netlink_flags1 = OBJ_UDATA_FLAG_OWN;
  monitor * rc_udev_monitor_new_from_netlink1;
  this1 = obj_type_context_check(L,1);
  name2 = luaL_optlstring(L,2,"udev",&(name_len2));
  rc_udev_monitor_new_from_netlink1 = udev_monitor_new_from_netlink(this1, name2);
  obj_type_monitor_push(L, rc_udev_monitor_new_from_netlink1, rc_udev_monitor_new_from_netlink_flags1);
  return 1;
}

/* method: enumerate */
static int context__enumerate__meth(lua_State *L) {
  context * this1;
  int rc_udev_enumerate_new_flags1 = OBJ_UDATA_FLAG_OWN;
  enumerate * rc_udev_enumerate_new1;
  this1 = obj_type_context_check(L,1);
  rc_udev_enumerate_new1 = udev_enumerate_new(this1);
  obj_type_enumerate_push(L, rc_udev_enumerate_new1, rc_udev_enumerate_new_flags1);
  return 1;
}

/* method: _priv */
static int device__delete__meth(lua_State *L) {
  int this_flags1 = 0;
  device * this1;
  this1 = obj_type_device_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  udev_device_unref(this1);
  return 0;
}

/* method: __tostring */
static int device____tostring__meth(lua_State *L) {
  device * this1;
  this1 = obj_type_device_check(L,1);
      lua_pushfstring (L, "device: %s (%s, %d)", 
        udev_device_get_devpath (this1),
        udev_device_get_subsystem (this1),
        udev_device_get_devnum (this1));
    
  return 1;
}

/* method: get_context */
static int device__get_context__meth(lua_State *L) {
  device * this1;
  context * rc_udev_device_get_udev1;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_udev1 = udev_device_get_udev(this1);
  obj_type_context_push(L, rc_udev_device_get_udev1, 0);
  return 1;
}

/* method: get_parent */
static int device__get_parent__meth(lua_State *L) {
  device * this1;
  device * rc_udev_device_get_parent1;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_parent1 = udev_device_get_parent(this1);
  obj_type_device_push(L, rc_udev_device_get_parent1, 0);
  return 1;
}

/* method: get_parent_with_subsystem_devtype */
static int device__get_parent_with_subsystem_devtype__meth(lua_State *L) {
  device * this1;
  size_t subsystem_len2;
  const char * subsystem2;
  size_t devtype_len3;
  const char * devtype3;
  device * rc_udev_device_get_parent_with_subsystem_devtype1;
  this1 = obj_type_device_check(L,1);
  subsystem2 = luaL_checklstring(L,2,&(subsystem_len2));
  devtype3 = luaL_optlstring(L,3,NULL,&(devtype_len3));
  rc_udev_device_get_parent_with_subsystem_devtype1 = udev_device_get_parent_with_subsystem_devtype(this1, subsystem2, devtype3);
  obj_type_device_push(L, rc_udev_device_get_parent_with_subsystem_devtype1, 0);
  return 1;
}

/* method: get_devpath */
static int device__get_devpath__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_devpath1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_devpath1 = udev_device_get_devpath(this1);
  lua_pushstring(L, rc_udev_device_get_devpath1);
  return 1;
}

/* method: get_subsystem */
static int device__get_subsystem__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_subsystem1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_subsystem1 = udev_device_get_subsystem(this1);
  lua_pushstring(L, rc_udev_device_get_subsystem1);
  return 1;
}

/* method: get_devtype */
static int device__get_devtype__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_devtype1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_devtype1 = udev_device_get_devtype(this1);
  lua_pushstring(L, rc_udev_device_get_devtype1);
  return 1;
}

/* method: get_syspath */
static int device__get_syspath__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_syspath1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_syspath1 = udev_device_get_syspath(this1);
  lua_pushstring(L, rc_udev_device_get_syspath1);
  return 1;
}

/* method: get_sysname */
static int device__get_sysname__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_sysname1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_sysname1 = udev_device_get_sysname(this1);
  lua_pushstring(L, rc_udev_device_get_sysname1);
  return 1;
}

/* method: get_sysnum */
static int device__get_sysnum__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_sysnum1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_sysnum1 = udev_device_get_sysnum(this1);
  lua_pushstring(L, rc_udev_device_get_sysnum1);
  return 1;
}

/* method: get_devnode */
static int device__get_devnode__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_devnode1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_devnode1 = udev_device_get_devnode(this1);
  lua_pushstring(L, rc_udev_device_get_devnode1);
  return 1;
}

/* method: get_is_initialized */
static int device__get_is_initialized__meth(lua_State *L) {
  device * this1;
  int rc_udev_device_get_is_initialized1 = 0;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_is_initialized1 = udev_device_get_is_initialized(this1);
  lua_pushinteger(L, rc_udev_device_get_is_initialized1);
  return 1;
}

/* method: get_property_value */
static int device__get_property_value__meth(lua_State *L) {
  device * this1;
  size_t key_len2;
  const char * key2;
  const char * rc_udev_device_get_property_value1 = NULL;
  this1 = obj_type_device_check(L,1);
  key2 = luaL_checklstring(L,2,&(key_len2));
  rc_udev_device_get_property_value1 = udev_device_get_property_value(this1, key2);
  lua_pushstring(L, rc_udev_device_get_property_value1);
  return 1;
}

/* method: get_driver */
static int device__get_driver__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_driver1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_driver1 = udev_device_get_driver(this1);
  lua_pushstring(L, rc_udev_device_get_driver1);
  return 1;
}

/* method: get_devnum */
static int device__get_devnum__meth(lua_State *L) {
  device * this1;
  dev_t rc_udev_device_get_devnum1;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_devnum1 = udev_device_get_devnum(this1);
  lua_pushinteger(L, rc_udev_device_get_devnum1);
  return 1;
}

/* method: get_action */
static int device__get_action__meth(lua_State *L) {
  device * this1;
  const char * rc_udev_device_get_action1 = NULL;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_action1 = udev_device_get_action(this1);
  lua_pushstring(L, rc_udev_device_get_action1);
  return 1;
}

/* method: get_sysattr_value */
static int device__get_sysattr_value__meth(lua_State *L) {
  device * this1;
  size_t sysattr_len2;
  const char * sysattr2;
  const char * rc_udev_device_get_sysattr_value1 = NULL;
  this1 = obj_type_device_check(L,1);
  sysattr2 = luaL_checklstring(L,2,&(sysattr_len2));
  rc_udev_device_get_sysattr_value1 = udev_device_get_sysattr_value(this1, sysattr2);
  lua_pushstring(L, rc_udev_device_get_sysattr_value1);
  return 1;
}

/* method: get_seqnum */
static int device__get_seqnum__meth(lua_State *L) {
  device * this1;
  unsigned long long int rc_udev_device_get_seqnum1;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_seqnum1 = udev_device_get_seqnum(this1);
  lua_pushinteger(L, rc_udev_device_get_seqnum1);
  return 1;
}

/* method: get_usec_since_initialized */
static int device__get_usec_since_initialized__meth(lua_State *L) {
  device * this1;
  unsigned long long int rc_udev_device_get_usec_since_initialized1;
  this1 = obj_type_device_check(L,1);
  rc_udev_device_get_usec_since_initialized1 = udev_device_get_usec_since_initialized(this1);
  lua_pushinteger(L, rc_udev_device_get_usec_since_initialized1);
  return 1;
}

/* method: _priv */
static int monitor__delete__meth(lua_State *L) {
  int this_flags1 = 0;
  monitor * this1;
  this1 = obj_type_monitor_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  udev_monitor_unref(this1);
  return 0;
}

/* method: get_context */
static int monitor__get_context__meth(lua_State *L) {
  monitor * this1;
  context * rc_udev_monitor_get_udev1;
  this1 = obj_type_monitor_check(L,1);
  rc_udev_monitor_get_udev1 = udev_monitor_get_udev(this1);
  obj_type_context_push(L, rc_udev_monitor_get_udev1, 0);
  return 1;
}

/* method: enable_receiving */
static int monitor__enable_receiving__meth(lua_State *L) {
  monitor * this1;
  err_rc rc_udev_monitor_enable_receiving1 = 0;
  this1 = obj_type_monitor_check(L,1);
  rc_udev_monitor_enable_receiving1 = udev_monitor_enable_receiving(this1);
  /* check for error. */
  if((rc_udev_monitor_enable_receiving1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_monitor_enable_receiving1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: getfd */
static int monitor__getfd__meth(lua_State *L) {
  monitor * this1;
  int rc_udev_monitor_get_fd1 = 0;
  this1 = obj_type_monitor_check(L,1);
  rc_udev_monitor_get_fd1 = udev_monitor_get_fd(this1);
  lua_pushinteger(L, rc_udev_monitor_get_fd1);
  return 1;
}

/* method: receive_device */
static int monitor__receive_device__meth(lua_State *L) {
  monitor * this1;
  int rc_udev_monitor_receive_device_flags1 = OBJ_UDATA_FLAG_OWN;
  device * rc_udev_monitor_receive_device1;
  this1 = obj_type_monitor_check(L,1);
  rc_udev_monitor_receive_device1 = udev_monitor_receive_device(this1);
  obj_type_device_push(L, rc_udev_monitor_receive_device1, rc_udev_monitor_receive_device_flags1);
  return 1;
}

/* method: filter_add_match_subsystem_devtype */
static int monitor__filter_add_match_subsystem_devtype__meth(lua_State *L) {
  monitor * this1;
  size_t subsystem_len2;
  const char * subsystem2;
  size_t devtype_len3;
  const char * devtype3;
  err_rc rc_udev_monitor_filter_add_match_subsystem_devtype1 = 0;
  this1 = obj_type_monitor_check(L,1);
  subsystem2 = luaL_checklstring(L,2,&(subsystem_len2));
  devtype3 = luaL_optlstring(L,3,NULL,&(devtype_len3));
  rc_udev_monitor_filter_add_match_subsystem_devtype1 = udev_monitor_filter_add_match_subsystem_devtype(this1, subsystem2, devtype3);
  /* check for error. */
  if((rc_udev_monitor_filter_add_match_subsystem_devtype1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_monitor_filter_add_match_subsystem_devtype1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: filter_add_match_tag */
static int monitor__filter_add_match_tag__meth(lua_State *L) {
  monitor * this1;
  size_t tag_len2;
  const char * tag2;
  err_rc rc_udev_monitor_filter_add_match_tag1 = 0;
  this1 = obj_type_monitor_check(L,1);
  tag2 = luaL_checklstring(L,2,&(tag_len2));
  rc_udev_monitor_filter_add_match_tag1 = udev_monitor_filter_add_match_tag(this1, tag2);
  /* check for error. */
  if((rc_udev_monitor_filter_add_match_tag1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_monitor_filter_add_match_tag1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: filter_update */
static int monitor__filter_update__meth(lua_State *L) {
  monitor * this1;
  err_rc rc_udev_monitor_filter_update1 = 0;
  this1 = obj_type_monitor_check(L,1);
  rc_udev_monitor_filter_update1 = udev_monitor_filter_update(this1);
  /* check for error. */
  if((rc_udev_monitor_filter_update1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_monitor_filter_update1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: filter_remove */
static int monitor__filter_remove__meth(lua_State *L) {
  monitor * this1;
  err_rc rc_udev_monitor_filter_remove1 = 0;
  this1 = obj_type_monitor_check(L,1);
  rc_udev_monitor_filter_remove1 = udev_monitor_filter_remove(this1);
  /* check for error. */
  if((rc_udev_monitor_filter_remove1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_monitor_filter_remove1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: _priv */
static int enumerate__delete__meth(lua_State *L) {
  int this_flags1 = 0;
  enumerate * this1;
  this1 = obj_type_enumerate_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  udev_enumerate_unref(this1);
  return 0;
}

/* method: get_context */
static int enumerate__get_context__meth(lua_State *L) {
  enumerate * this1;
  context * rc_udev_enumerate_get_udev1;
  this1 = obj_type_enumerate_check(L,1);
  rc_udev_enumerate_get_udev1 = udev_enumerate_get_udev(this1);
  obj_type_context_push(L, rc_udev_enumerate_get_udev1, 0);
  return 1;
}

/* method: add_match_subsystem */
static int enumerate__add_match_subsystem__meth(lua_State *L) {
  enumerate * this1;
  size_t subsystem_len2;
  const char * subsystem2;
  err_rc rc_udev_enumerate_add_match_subsystem1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  subsystem2 = luaL_checklstring(L,2,&(subsystem_len2));
  rc_udev_enumerate_add_match_subsystem1 = udev_enumerate_add_match_subsystem(this1, subsystem2);
  /* check for error. */
  if((rc_udev_enumerate_add_match_subsystem1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_subsystem1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_nomatch_subsystem */
static int enumerate__add_nomatch_subsystem__meth(lua_State *L) {
  enumerate * this1;
  size_t subsystem_len2;
  const char * subsystem2;
  err_rc rc_udev_enumerate_add_nomatch_subsystem1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  subsystem2 = luaL_checklstring(L,2,&(subsystem_len2));
  rc_udev_enumerate_add_nomatch_subsystem1 = udev_enumerate_add_nomatch_subsystem(this1, subsystem2);
  /* check for error. */
  if((rc_udev_enumerate_add_nomatch_subsystem1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_nomatch_subsystem1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_match_sysattr */
static int enumerate__add_match_sysattr__meth(lua_State *L) {
  enumerate * this1;
  size_t sysattr_len2;
  const char * sysattr2;
  size_t value_len3;
  const char * value3;
  err_rc rc_udev_enumerate_add_match_sysattr1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  sysattr2 = luaL_checklstring(L,2,&(sysattr_len2));
  value3 = luaL_checklstring(L,3,&(value_len3));
  rc_udev_enumerate_add_match_sysattr1 = udev_enumerate_add_match_sysattr(this1, sysattr2, value3);
  /* check for error. */
  if((rc_udev_enumerate_add_match_sysattr1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_sysattr1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_nomatch_sysattr */
static int enumerate__add_nomatch_sysattr__meth(lua_State *L) {
  enumerate * this1;
  size_t sysattr_len2;
  const char * sysattr2;
  size_t value_len3;
  const char * value3;
  err_rc rc_udev_enumerate_add_nomatch_sysattr1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  sysattr2 = luaL_checklstring(L,2,&(sysattr_len2));
  value3 = luaL_checklstring(L,3,&(value_len3));
  rc_udev_enumerate_add_nomatch_sysattr1 = udev_enumerate_add_nomatch_sysattr(this1, sysattr2, value3);
  /* check for error. */
  if((rc_udev_enumerate_add_nomatch_sysattr1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_nomatch_sysattr1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_match_property */
static int enumerate__add_match_property__meth(lua_State *L) {
  enumerate * this1;
  size_t property_len2;
  const char * property2;
  size_t value_len3;
  const char * value3;
  err_rc rc_udev_enumerate_add_match_property1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  property2 = luaL_checklstring(L,2,&(property_len2));
  value3 = luaL_checklstring(L,3,&(value_len3));
  rc_udev_enumerate_add_match_property1 = udev_enumerate_add_match_property(this1, property2, value3);
  /* check for error. */
  if((rc_udev_enumerate_add_match_property1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_property1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_match_tag */
static int enumerate__add_match_tag__meth(lua_State *L) {
  enumerate * this1;
  size_t tag_len2;
  const char * tag2;
  err_rc rc_udev_enumerate_add_match_tag1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  tag2 = luaL_checklstring(L,2,&(tag_len2));
  rc_udev_enumerate_add_match_tag1 = udev_enumerate_add_match_tag(this1, tag2);
  /* check for error. */
  if((rc_udev_enumerate_add_match_tag1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_tag1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_match_parent */
static int enumerate__add_match_parent__meth(lua_State *L) {
  enumerate * this1;
  device * parent2;
  err_rc rc_udev_enumerate_add_match_parent1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  parent2 = obj_type_device_check(L,2);
  rc_udev_enumerate_add_match_parent1 = udev_enumerate_add_match_parent(this1, parent2);
  /* check for error. */
  if((rc_udev_enumerate_add_match_parent1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_parent1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_match_is_initialized */
static int enumerate__add_match_is_initialized__meth(lua_State *L) {
  enumerate * this1;
  err_rc rc_udev_enumerate_add_match_is_initialized1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  rc_udev_enumerate_add_match_is_initialized1 = udev_enumerate_add_match_is_initialized(this1);
  /* check for error. */
  if((rc_udev_enumerate_add_match_is_initialized1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_is_initialized1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_match_sysname */
static int enumerate__add_match_sysname__meth(lua_State *L) {
  enumerate * this1;
  size_t sysname_len2;
  const char * sysname2;
  err_rc rc_udev_enumerate_add_match_sysname1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  sysname2 = luaL_checklstring(L,2,&(sysname_len2));
  rc_udev_enumerate_add_match_sysname1 = udev_enumerate_add_match_sysname(this1, sysname2);
  /* check for error. */
  if((rc_udev_enumerate_add_match_sysname1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_match_sysname1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: add_syspath */
static int enumerate__add_syspath__meth(lua_State *L) {
  enumerate * this1;
  size_t syspath_len2;
  const char * syspath2;
  err_rc rc_udev_enumerate_add_syspath1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  syspath2 = luaL_checklstring(L,2,&(syspath_len2));
  rc_udev_enumerate_add_syspath1 = udev_enumerate_add_syspath(this1, syspath2);
  /* check for error. */
  if((rc_udev_enumerate_add_syspath1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_add_syspath1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: scan_devices */
static int enumerate__scan_devices__meth(lua_State *L) {
  enumerate * this1;
  err_rc rc_udev_enumerate_scan_devices1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  rc_udev_enumerate_scan_devices1 = udev_enumerate_scan_devices(this1);
  /* check for error. */
  if((rc_udev_enumerate_scan_devices1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_scan_devices1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: scan_subsystems */
static int enumerate__scan_subsystems__meth(lua_State *L) {
  enumerate * this1;
  err_rc rc_udev_enumerate_scan_subsystems1 = 0;
  this1 = obj_type_enumerate_check(L,1);
  rc_udev_enumerate_scan_subsystems1 = udev_enumerate_scan_subsystems(this1);
  /* check for error. */
  if((rc_udev_enumerate_scan_subsystems1 < 0)) {
    lua_pushnil(L);
      error_code__err_rc__push(L, rc_udev_enumerate_scan_subsystems1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: get_list */
static int enumerate__get_list__meth(lua_State *L) {
  enumerate * this1;
  this1 = obj_type_enumerate_check(L,1);
      lua_newtable (L);
      struct udev_list_entry *list = udev_enumerate_get_list_entry(this1), *entry;
      int i = 1;
      udev_list_entry_foreach(entry, list) {
        const char *path = udev_list_entry_get_name(entry);
        lua_pushstring (L, path);
        lua_rawseti (L, -2, i++);
      }
    
  return 1;
}



static const luaL_Reg obj_Errors_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_Errors_methods[] = {
  {"description", Errors__description__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_Errors_metas[] = {
  {NULL, NULL}
};

static const obj_const obj_Errors_constants[] = {
#ifdef EINVAL
  {"EINVAL", NULL, EINVAL, CONST_NUMBER},
#endif
#ifdef ENOMEM
  {"ENOMEM", NULL, ENOMEM, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_Errors_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_context_pub_funcs[] = {
  {"new", context__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_context_methods[] = {
  {"get_log_priority", context__get_log_priority__meth},
  {"set_log_priority", context__set_log_priority__meth},
  {"get_dev_path", context__get_dev_path__meth},
  {"get_sys_path", context__get_sys_path__meth},
  {"get_run_path", context__get_run_path__meth},
  {"device_from_syspath", context__device_from_syspath__meth},
  {"device_from_devnum", context__device_from_devnum__meth},
  {"device_from_subsystem_sysname", context__device_from_subsystem_sysname__meth},
  {"device_from_environment", context__device_from_environment__meth},
  {"monitor", context__monitor__meth},
  {"enumerate", context__enumerate__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_context_metas[] = {
  {"__gc", context__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_context_bases[] = {
  {-1, NULL}
};

static const obj_field obj_context_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_context_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_context_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_device_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_device_methods[] = {
  {"get_context", device__get_context__meth},
  {"get_parent", device__get_parent__meth},
  {"get_parent_with_subsystem_devtype", device__get_parent_with_subsystem_devtype__meth},
  {"get_devpath", device__get_devpath__meth},
  {"get_subsystem", device__get_subsystem__meth},
  {"get_devtype", device__get_devtype__meth},
  {"get_syspath", device__get_syspath__meth},
  {"get_sysname", device__get_sysname__meth},
  {"get_sysnum", device__get_sysnum__meth},
  {"get_devnode", device__get_devnode__meth},
  {"get_is_initialized", device__get_is_initialized__meth},
  {"get_property_value", device__get_property_value__meth},
  {"get_driver", device__get_driver__meth},
  {"get_devnum", device__get_devnum__meth},
  {"get_action", device__get_action__meth},
  {"get_sysattr_value", device__get_sysattr_value__meth},
  {"get_seqnum", device__get_seqnum__meth},
  {"get_usec_since_initialized", device__get_usec_since_initialized__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_device_metas[] = {
  {"__gc", device__delete__meth},
  {"__tostring", device____tostring__meth},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_device_bases[] = {
  {-1, NULL}
};

static const obj_field obj_device_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_device_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_device_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_monitor_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_monitor_methods[] = {
  {"get_context", monitor__get_context__meth},
  {"enable_receiving", monitor__enable_receiving__meth},
  {"getfd", monitor__getfd__meth},
  {"receive_device", monitor__receive_device__meth},
  {"filter_add_match_subsystem_devtype", monitor__filter_add_match_subsystem_devtype__meth},
  {"filter_add_match_tag", monitor__filter_add_match_tag__meth},
  {"filter_update", monitor__filter_update__meth},
  {"filter_remove", monitor__filter_remove__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_monitor_metas[] = {
  {"__gc", monitor__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_monitor_bases[] = {
  {-1, NULL}
};

static const obj_field obj_monitor_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_monitor_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_monitor_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_enumerate_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_enumerate_methods[] = {
  {"get_context", enumerate__get_context__meth},
  {"add_match_subsystem", enumerate__add_match_subsystem__meth},
  {"add_nomatch_subsystem", enumerate__add_nomatch_subsystem__meth},
  {"add_match_sysattr", enumerate__add_match_sysattr__meth},
  {"add_nomatch_sysattr", enumerate__add_nomatch_sysattr__meth},
  {"add_match_property", enumerate__add_match_property__meth},
  {"add_match_tag", enumerate__add_match_tag__meth},
  {"add_match_parent", enumerate__add_match_parent__meth},
  {"add_match_is_initialized", enumerate__add_match_is_initialized__meth},
  {"add_match_sysname", enumerate__add_match_sysname__meth},
  {"add_syspath", enumerate__add_syspath__meth},
  {"scan_devices", enumerate__scan_devices__meth},
  {"scan_subsystems", enumerate__scan_subsystems__meth},
  {"get_list", enumerate__get_list__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_enumerate_metas[] = {
  {"__gc", enumerate__delete__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_enumerate_bases[] = {
  {-1, NULL}
};

static const obj_field obj_enumerate_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_enumerate_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_enumerate_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg udev_function[] = {
  {NULL, NULL}
};

static const obj_const udev_constants[] = {
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_Errors), REG_META, obj_Errors_pub_funcs, obj_Errors_methods, obj_Errors_metas, NULL, NULL, obj_Errors_constants, NULL, 1},
  { &(obj_type_context), REG_OBJECT, obj_context_pub_funcs, obj_context_methods, obj_context_metas, obj_context_bases, obj_context_fields, obj_context_constants, obj_context_implements, 0},
  { &(obj_type_device), REG_OBJECT, obj_device_pub_funcs, obj_device_methods, obj_device_metas, obj_device_bases, obj_device_fields, obj_device_constants, obj_device_implements, 0},
  { &(obj_type_monitor), REG_OBJECT, obj_monitor_pub_funcs, obj_monitor_methods, obj_monitor_metas, obj_monitor_bases, obj_monitor_fields, obj_monitor_constants, obj_monitor_implements, 0},
  { &(obj_type_enumerate), REG_OBJECT, obj_enumerate_pub_funcs, obj_enumerate_methods, obj_enumerate_metas, obj_enumerate_bases, obj_enumerate_fields, obj_enumerate_constants, obj_enumerate_implements, 0},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0}
};





#if LUAJIT_FFI
static const ffi_export_symbol udev_ffi_export[] = {
  {NULL, { NULL } }
};
#endif




static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_udev(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

	/* register interfaces */
	obj_register_interfaces(L, obj_interfaces);

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
	lua_pushlightuserdata(L, obj_udata_private_key);
	lua_pushvalue(L, priv_table);
	lua_rawset(L, LUA_REGISTRYINDEX);  /* store private table in registry. */

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
#if REG_MODULES_AS_GLOBALS
	luaL_register(L, "udev", udev_function);
#else
	lua_newtable(L);
	luaL_setfuncs(L, udev_function, 0);
#endif

	/* register module constants. */
	obj_type_register_constants(L, udev_constants, -1, 0);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	if(nobj_check_ffi_support(L)) {
		nobj_try_loading_ffi(L, "udev.nobj.ffi.lua", udev_ffi_lua_code,
			udev_ffi_export, priv_table);
	}
#endif

	/* Cache reference to epoll.Errors table for errno->string convertion. */
	lua_pushlightuserdata(L, udev_Errors_key);
	lua_getfield(L, -2, "Errors");
	lua_rawset(L, LUA_REGISTRYINDEX);



	return 1;
}


