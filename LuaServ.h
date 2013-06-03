#ifndef __LUA_MANAGER_LUASERV__
#define __LUA_MANAGER_LUASERV__
#include <string>
#include <map>


#include "LuaState.h"
#include "ConfigId.h"

namespace lua {
namespace manager {


class LuaServ {
  LuaState st_;
  const ConfigId &cfg_;
  int last_upflag_; // 最后更新时间
  int check_stamp_;
  std::map<int, std::string> id2sha1_; // luastate 目前加载使用的脚本sha1值
  



  bool load_script(std::string &err);
  bool load_one(int id, const char *path, const char *sha1, std::string &err);

 public:
 LuaServ(const ConfigId &cfg)
   : cfg_(cfg), last_upflag_(0), check_stamp_(0)
  {}


  void reg_fun(const char *lib_name, const struct luaL_Reg *fun_list) { st_.reg_fun(lib_name, fun_list); }
  void reg_var(void (*cb)(lua_State *, void *), void *arg) { st_.reg_var(cb, arg); }

  bool load(std::string &err);
  void clear_stack() { st_.clear_stack(); }

  lua_State *stack_fun(const char *tb, const char *fun, std::string &err);
  lua_State *stack_var(const char *tb, const char *var, std::string &err);

  LuaState &state() { return st_; }

};

};
};


#endif
