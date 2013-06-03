#include "LuaState.h"

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

namespace lua {
namespace manager {


static int64_t _int64(lua_State *L, int index) {
	int type = lua_type(L,index);
	int64_t n = 0;
	switch(type) {
	case LUA_TNUMBER: {
		lua_Number d = lua_tonumber(L,index);
		n = (int64_t)d;
		break;
	}
	case LUA_TSTRING: {
    const char *str = lua_tostring(L, index);
		n = atol(str);
		break;
	}
	case LUA_TLIGHTUSERDATA: {
    //void * p = luaL_checkudata(L,index,INT64_META_KEY);
		//n = luaE_toint64(L, index);
    n = luaE_checkint64(L, index);
		break;
	}
	default:
		return luaL_error(L, "argument %d error type %s", index, lua_typename(L,type));
	}
	return n;
}


static int
int64_add(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	luaE_pushint64(L, a+b);
	
	return 1;
}

static int int64_sub(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	luaE_pushint64(L, a-b);
	
	return 1;
}

static int int64_mul(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	luaE_pushint64(L, a * b);
	
	return 1;
}

static int int64_div(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	if (b == 0) {
		return luaL_error(L, "div by zero");
	}
	luaE_pushint64(L, a / b);
	
	return 1;
}

static int int64_mod(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	if (b == 0) {
		return luaL_error(L, "mod by zero");
	}
	luaE_pushint64(L, a % b);
	
	return 1;
}

static int64_t
_pow64(int64_t a, int64_t b) {
	if (b == 1) {
		return a;
	}
	int64_t a2 = a * a;
	if (b % 2 == 1) {
		return _pow64(a2, b/2) * a;
	} else {
		return _pow64(a2, b/2);
	}
}

static int
int64_pow(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	int64_t p;
	if (b > 0) {
		p = _pow64(a,b);
	} else if (b == 0) {
		p = 1;
	} else {
		return luaL_error(L, "pow by nagtive number %d",(int)b);
	} 
	luaE_pushint64(L, p);

	return 1;
}

static int
int64_unm(lua_State *L) {
	int64_t a = _int64(L,1);
	luaE_pushint64(L, -a);
	return 1;
}

static int
int64_new(lua_State *L) {
	int top = lua_gettop(L);
	int64_t n;
	switch(top) {
		case 0 : 
      luaE_pushint64(L,0);
			break;
		case 1 :
			n = _int64(L,1);
			luaE_pushint64(L,n);
			break;
		default: {
			int base = luaL_checkinteger(L,2);
			if (base < 2) {
				luaL_error(L, "base must be >= 2");
			}
			const char * str = luaL_checkstring(L, 1);
			n = strtoll(str, NULL, base);
			luaE_pushint64(L,n);
			break;
		}
	}

  luaL_getmetatable(L, INT64_META_KEY);
  lua_setmetatable(L, -2);


	return 1;
}

static int
int64_eq(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	printf("%s %s\n",lua_typename(L,1),lua_typename(L,2));
	printf("%ld %ld\n",a,b);
	lua_pushboolean(L,a == b);
	return 1;
}

static int
int64_lt(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	lua_pushboolean(L,a < b);
	return 1;
}

static int
int64_le(lua_State *L) {
	int64_t a = _int64(L,1);
	int64_t b = _int64(L,2);
	lua_pushboolean(L,a <= b);
	return 1;
}

static int
int64_len(lua_State *L) {
	int64_t a = _int64(L,1);
	lua_pushnumber(L,(lua_Number)a);
	return 1;
}

static int tostring(lua_State *L) {

  //int64_t n = luaE_toint64(L, 1);
  int64_t n = luaE_checkint64(L, 1);

  char buff[50] = {0};
  snprintf(buff, sizeof(buff), "%ld", n);


  lua_pushstring(L, buff);

	return 1;
}

static int tonumber(lua_State *L) {

  //int64_t n = luaE_toint64(L, 1);
  int64_t n = luaE_checkint64(L, 1);

  double d = (double)n;
  lua_pushnumber(L, d);

	return 1;
}


static const	luaL_Reg meta[] = {
  { "__add", int64_add },
  { "__sub", int64_sub },
  { "__mul", int64_mul },
  { "__div", int64_div },
  { "__mod", int64_mod },
  { "__unm", int64_unm },
  { "__pow", int64_pow },
  { "__eq", int64_eq },
  { "__lt", int64_lt },
  { "__le", int64_le },
  { "__len", int64_len },
  { "__tostring", tostring },
  { NULL, NULL },
};

static const luaL_Reg lib[] = {
  { "new", int64_new },
  { "tonumber", tonumber },
  { NULL, NULL },
};



int luaopen_int64(lua_State *L) {
  if (sizeof(intptr_t)!=sizeof(int64_t)) {
    return luaL_error(L, "Only support 64bit architecture");
  }
  luaL_newmetatable(L, INT64_META_KEY);
  luaL_register(L, NULL, meta);

  luaL_register(L, "int64", lib);

  lua_settop(L, 0);

	return 1;
}

};
};

