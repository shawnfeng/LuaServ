#include <stdio.h>

#include "LuaServ.h"

using namespace std;
using namespace lua::manager;

// 每次使用时候，都调用下load函数，检查是否有更新
bool LuaServ::load(std::string &err)
{
  //printf("LuaServ.load: lastup=%d, upflag=%d\n", last_upflag_, cfg_.upflag());
  if (last_upflag_ < cfg_.upflag()) {
    // 这里也加个间隔控制，因为如果某个脚本有错误的花，last_flag将一直不给更新
    // 这样造成，每次都要所有脚本检测了
    int stamp = time(NULL);
    int bef_stamp = check_stamp_;
    int intv = cfg_.get_check_intv();
    //printf("LuaServ.load: bef=%d, now=%d, now-bef=%d intv=%d\n", bef_stamp, stamp, stamp - bef_stamp, intv);
    if (stamp - bef_stamp > intv) {
      check_stamp_ = stamp;
      if (!load_script(err)) {
        return false;
      }
      last_upflag_ = cfg_.upflag();  
    }
  }

  return true;
}


bool LuaServ::load_script(string &err)
{

  // 开始加载
  map<int, pair<string, string> > infos;
  cfg_.get_serv_script(infos);

  string path;
  string root;

  for (map<int, pair<string, string> >::const_iterator it = infos.begin(); it != infos.end(); ++it) {
    const pair<string, string> ifo = it->second;

    if (!load_one(it->first, ifo.first.c_str(), ifo.second.c_str(), err)) {
      return false;
    }
  }
  return true;
}

bool LuaServ::load_one(int id, const char *path, const char *sha1, std::string &err)
{
  map<int, string>::const_iterator it = id2sha1_.find(id);
  if (it == id2sha1_.end() || it->second != sha1) {
    if (!st_.load_file(path, err) || !st_.pcall(err)) {
      return false;
    }

    id2sha1_[id] = sha1;
  }
  return true;
}


lua_State *LuaServ::stack_fun(const char *tb, const char *fun, string &err)
{
  // 延时加载，在调用的时候，检查脚本有没有更新
  if (!load(err)) {
    return NULL;
  }

  lua_State *L = st_.st();
  lua_getglobal(L, tb);
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1); // 从栈中弹出一个元素,恢复到加载前的状态
    err = tb;
    err += " is not a table";
    return NULL;
  }

  lua_getfield(L, -1, fun);
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 1);
    lua_pop(L, 1);
    err = fun;
    err += " is not a function";
    return NULL;
  }

  // 移除table
  lua_remove(L, -2);

  return L;

}


lua_State *LuaServ::stack_var(const char *tb, const char *var, string &err)
{
  // 延时加载，在调用的时候，检查脚本有没有更新
  if (!load(err)) {
    return NULL;
  }


  lua_State *L = st_.st();
  //st_.stackDump("stack_var: 0");
  lua_getglobal(L, tb);
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1); // 从栈中弹出一个元素,恢复到加载前的状态
    err = tb;
    err += " is not a table";
    return NULL;
  }

  lua_getfield(L, -1, var);
  //st_.stackDump("stack_var: 1");


  // 移除table
  lua_remove(L, -2);
  //st_.stackDump("stack_var: 2");

  return L;

}


