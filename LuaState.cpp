#include "LuaState.h"
using namespace std;
using namespace lua::manager;

void LuaState::stackDump(const char *pref) const
{
  int i;
  int top = lua_gettop(L);
  printf("[%s] stack=%d\n", pref, top);
  for (i=1; i <= top; ++i) {
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TSTRING: {
      printf("[%s] %d: '%s'", pref, i, lua_tostring(L, i));
      break;
    }
    case LUA_TBOOLEAN: {
      printf("[%s] %d: '%s'", pref, i, lua_toboolean(L, i) ? "true" : "false");
      break;
    }
    case LUA_TNUMBER: {
      printf("[%s] %d: %g", pref, i, lua_tonumber(L, i));
      break;
    }
    default: {
      printf("[%s]%d, '%s'", pref, i, lua_typename(L, t));
      break;
    }

    }
    printf("   ");
  }
  printf("\n");

}

bool LuaState::load_fun(const char *body, const char *funname, std::string &err)
{
  string fun = "function ";
  fun += funname;
  fun += "() ";
  fun += body;
  fun += " end";

  if (luaL_loadbuffer(L, fun.c_str(), fun.size(), funname)) {
    char buff[500] = {0};
    snprintf(buff, sizeof(buff) - 1, "cannot compile function: %s %s", funname, lua_tostring(L, -1));
    lua_pop(L, 1); // 从栈中弹出一个元素,恢复到加载前的状态
    return false;
  } else {
    return true;
  }

}

bool LuaState::load_file(const char *path, std::string &err)
{
  //stackDump(path);
  if (luaL_loadfile(L, path)) {
    char buff[500] = {0};
    snprintf(buff, sizeof(buff) - 1, "cannot compile config. file: %s", lua_tostring(L, -1));
    lua_pop(L, 1); // 从栈中弹出一个元素,恢复到加载前的状态
    //stackDump(path);
    err = buff;
    return false;

  } else {
    //stackDump(path);
    return true;
  }
}

bool LuaState::pcall(std::string &err)
{
  //stackDump("call 0");
  if (lua_pcall(L, 0, 0, 0)) {
    char buff[500] = {0};
    snprintf(buff, sizeof(buff) - 1, "cannot run lua file: %s", lua_tostring(L, -1));
    lua_pop(L, 1); // 从栈中弹出一个元素,恢复到加载前的状态
    //stackDump("call 1");
    err = buff;
    return false;

  } else {
    //stackDump("call 1");
    return true;
  }
}
