#ifndef __LUA_MANAGER_LUASTATE__
#define __LUA_MANAGER_LUASTATE__

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>
#include <map>



#define INT64_META_KEY "INT64-1c882271-3758-428b-96f6"
namespace lua {
namespace manager {

int luaopen_int64(lua_State *L);

inline void luaE_pushint64(lua_State *L, int64_t n) {
	void *p = (void *)(intptr_t)n;
	lua_pushlightuserdata(L, p);
}

inline int64_t luaE_toint64(lua_State *L, int index) {
  void *p = lua_touserdata(L, index);
  return (int64_t)p;
}

inline int64_t luaE_checkint64(lua_State *L, int index) {
  //void *p = luaL_checkudata(L,index,INT64_META_KEY);
  //int type = lua_type(L, index);
  //printf("-----------type: %d\n", type);
  //printf("-----------type: %d %d\n", LUA_TSTRING, LUA_TLIGHTUSERDATA);

  luaL_checktype(L, index, LUA_TLIGHTUSERDATA);
  return luaE_toint64(L, index);
}

inline int luaE_isint64(lua_State *L, int index) {

  //  printf("-----------type: %d\n", type);
  //  printf("-----------type: %d %d %d\n", LUA_TSTRING, LUA_TLIGHTUSERDATA, LUA_TUSERDATA);

  return lua_islightuserdata(L, index);

}

// ------------------------------其他类辅助函数
inline int luaE_errreturn(lua_State *L, const char *err)
{
  lua_pushnil(L);
  lua_pushstring(L, err);
  return 2;
}

// table -- > map<string, string> 映射
inline bool luaE_trytablemap(lua_State *L, int index, std::map<std::string, std::string> &rv, std::string &err)
{
  if (!lua_istable(L, index)) {
    err = "map<string, string> can only changed by table";
    return false;
  }

  lua_pushnil(L);
  for (;lua_next(L, index) != 0;) {
    /* uses 'key' (at index -2) and 'value' (at index -1) */
    if (!lua_isstring(L, -2)) {
      return false;
    }
    //printf("value=%s\n", lua_tostring(L, -1));
    if (lua_isstring(L, -1)) {
      rv[lua_tostring(L, -2)] = lua_tostring(L, -1);

    // 对扩展的int64类型特殊处理一下
    } else if (luaE_isint64(L, -1)) {
      char buff[30] = {0};
      snprintf(buff, sizeof(buff)-1, "%ld", luaE_toint64(L, -1));
      rv[lua_tostring(L, -2)] = buff;
    }
    /* removes 'value'; keeps 'key' for next iteration */
    lua_pop(L, 1);
  }


  return true;

}


// ----------------只进行检查，如果出错返回错误，但是不raise error
inline bool luaE_tryfieldstring(lua_State *L, int index, const char *field, std::string &rv, std::string &err)
{
  bool ok = true;
  lua_getfield(L, index, field);
  if (lua_isstring(L, -1)) {
    rv = lua_tostring(L, -1);
  } else {
    err = field;
    err += " of table except string";
    ok = false;
  }

  lua_pop(L, 1);
  return ok;
}

inline bool luaE_tryfieldint(lua_State *L, int index, const char *field, int &rv, std::string &err)
{
  bool ok = true;
  lua_getfield(L, index, field);
  if (lua_isnumber(L, -1)) {
    rv = lua_tointeger(L, -1);
  } else {
    err = field;
    err += " of table except int";
    ok = false;
  }

  lua_pop(L, 1);

  return ok;

}

inline bool luaE_tryfieldboolean(lua_State *L, int index, const char *field, bool &rv, std::string &err)
{
  bool ok = true;
  lua_getfield(L, index, field);
  if (lua_isboolean(L, -1)) {
    rv = (lua_toboolean(L, -1) ? true : false);
  } else {
    err = field;
    err += " of table except boolean";
    ok = false;

  }

  lua_pop(L, 1);
  return ok;

}


inline bool luaE_tryfieldint64(lua_State *L, int index, const char *field, int64_t &rv, std::string &err)
{
  bool ok = true;
  lua_getfield(L, index, field);
  if (luaE_isint64(L, -1)) {
    rv = luaE_toint64(L, -1);
  } else {
    err = field;
    err += " of table except int64";
    ok = false;

  }

  lua_pop(L, 1);
  return ok;

}

inline bool luaE_tryfieldtable(lua_State *L, int index, const char *field, std::string &err)
{
  bool ok = true;
  lua_getfield(L, index, field);
  if (!lua_istable(L, -1)) {
    err = field;
    err += " of table except table";
    ok = false;
    lua_pop(L, 1);
  }

  return ok;
}


//------------------------------field 类 get
inline void luaE_checkfieldstring(lua_State *L, int index, const char *field, std::string &rv)
{

  lua_getfield(L, index, field);
  if (!lua_isstring(L, -1)) {
    luaL_error(L, "invalid table field:%s type string excepted", field);
  }
  rv = lua_tostring(L, -1);
  lua_pop(L, 1);

}


inline int luaE_checkfieldint(lua_State *L, int index, const char *field)
{

  lua_getfield(L, index, field);
  if (!lua_isnumber(L, -1)) {
    luaL_error(L, "invalid table field:%s type number excepted", field);
  }
  int rv = lua_tointeger(L, -1);
  lua_pop(L, 1);

  return rv;

}

inline bool luaE_checkfieldboolean(lua_State *L, int index, const char *field)
{

  lua_getfield(L, index, field);
  if (!lua_isboolean(L, -1)) {
    luaL_error(L, "invalid table field:%s type boolean excepted", field);
  }
  int rv = lua_toboolean(L, -1);
  lua_pop(L, 1);

  return (rv == 0 ? false : true);

}


inline int64_t luaE_checkfieldint64(lua_State *L, int index, const char *field)
{

  lua_getfield(L, index, field);
  if (!luaE_isint64(L, -1)) {
    luaL_error(L, "invalid table field:%s type int64 excepted", field);
  }
  int64_t rv = luaE_toint64(L, -1);
  lua_pop(L, 1);

  return rv;

}

inline void luaE_checkfieldtable(lua_State *L, int index, const char *field)
{

  lua_getfield(L, index, field);
  if (!lua_istable(L, -1)) {
    luaL_error(L, "invalid table field:%s type table excepted", field);
  }

}


//------------------------------vector 类get
// void lua_rawgeti (lua_State *L, int index, int n);
// 把 t[n] 的值压栈, 这里的 t 是指给定索引 index 处的一个值。 这是一个直接访问;就是说,它不会触
// 发元方法

//------------------------------field 类push
inline void luaE_pushfieldstring(lua_State *L, const char *field, const char *v)
{
  lua_pushstring(L, field);
  lua_pushstring(L, v);
  lua_settable(L, -3);
}

inline void luaE_pushfieldint(lua_State *L, const char *field, int v)
{
  lua_pushstring(L, field);
  lua_pushinteger(L, v);
  lua_settable(L, -3);
}


inline void luaE_pushfieldint64(lua_State *L, const char *field, int64_t v)
{
  lua_pushstring(L, field);
  luaE_pushint64(L, v);
  lua_settable(L, -3);
}


inline void luaE_pushfieldboolean(lua_State *L, const char *field, bool v)
{
  lua_pushstring(L, field);
  lua_pushboolean(L, v);
  lua_settable(L, -3);
}

inline void luaE_pushfieldtable(lua_State *L, const char *field)
{
  lua_pushstring(L, field);
  lua_insert(L, -2);
  lua_settable(L, -3);

}


//------------------------------vector 类push
// void lua_rawseti (lua_State *L, int index, int n);
// 等价于 t[n] = v, 这里的 t 是指给定索引 index 处的一个值, 而 v 是栈顶的值。
// 函数将把这个值弹出栈。 赋值操作是直接的;就是说,不会触发元方法


//==============================
class LuaState {
  lua_State *L;

 public:

 LuaState() : L(NULL)
    { 
    L = luaL_newstate();
    luaL_openlibs(L); /* open standard lib, extend can add open local lib function*/
    luaopen_int64(L);
    }
  ~LuaState() { 
    if (!L) {
      lua_close(L);
      L = NULL;
    }
  }
  // debug function
  void stackDump(const char *pref) const;

  // -------------

  lua_State *st() { return L; }
  

  bool load_file(const char *path, std::string &err);
  bool load_fun(const char *body, const char *funname, std::string &err);
  bool pcall(std::string &err);
  void clear_stack() { lua_settop(L, 0); }
  // 内部函数导出
  void reg_fun(const char *lib_name, const struct luaL_Reg *fun_list) {
    luaL_register(L, lib_name, fun_list);
    lua_settop(L, 0);
  }
  // 变量导出
  void reg_var(void (*cb)(lua_State *, void *), void *arg) {
    cb(L, arg);
    lua_settop(L, 0);
  }
};

};
};

#endif
