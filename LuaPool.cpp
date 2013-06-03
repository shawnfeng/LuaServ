#include "LuaPool.h"

using namespace std;
using namespace lua::manager;




// -----------------------
void LuaPool::free(int type, LuaServ *lua)
{
  if (type < jobs_sum_) {
    job_count_[type]--;
    // 不管怎样，还回的状态，把栈清空
    lua->clear_stack();
    ScopedLock lock(mutex_);
    pool_.push_back(lua);
  }
}

LuaServ *LuaPool::alloc(int type)
{
  // 验证type类的job已经占用了多少state，超限返回NULL

  // 返回一个rrt类型的state指针，会自动归还
  if (type >= jobs_sum_) {

    return NULL;
  }


  ScopedLock lock(mutex_);
  if (!pool_.empty()) {
    LuaServ *p = pool_.back();
    pool_.pop_back();
    job_count_[type]++;
    return p;


  } else {
    return NULL;

  }
}

// 只在初始化时候调用一次，不加锁，其他时候调用后果自负
bool LuaPool::init(
                   const char **lib_name, const struct luaL_Reg **fun_list,
                   void (*cb)(lua_State *, void *), void *arg,
                   std::string &err
                   )
{

  bool rv = true;

  for (vector<LuaServ *>::iterator it = pool_.begin(); it != pool_.end(); ++it) {
    if (lib_name && fun_list) {
      for (int i = 0; lib_name[i] && fun_list[i]; ++i) {
        if (lib_name[i] && fun_list[i]) {
          (*it)->reg_fun(lib_name[i], fun_list[i]);
        }
      }
    }

    if (cb && arg) {
      (*it)->reg_var(cb, arg);
    }

  }

  for (vector<LuaServ *>::iterator it = pool_.begin(); it != pool_.end(); ++it) {
    if (!(*it)->load(err)) {
      rv = false;
      break;
    }
  }


  return rv;
}
/*
void LuaPool::check_config(void (*logfun)(bool, const std::list<std::string> &, const std::string &), list<string> &log, string &err)
{
  for (;;) {
    log.clear();
    bool isok = cfg_.check_change(log, err);
    logfun(isok, log, err);
    sleep(ONE_CHECK_INTV);
  }
}


void LuaPool::start(void (*logfun)(bool, const std::list<std::string> &, const std::string &)) {

  list<string> log;
  string err;
  bool isok = cfg_.check_change(log, err);
  logfun(isok, log, err);
  // init load
  for (vector<LuaServ *>::iterator it = pool_.begin(); it != pool_.end(); ++it) {
    isok = (*it)->load(err);
    logfun(isok, log, err);
    if (!isok) break;

  }


  //check_config(logfun, log, err);
  boost::thread td(boost::bind(&LuaPool::check_config, this, logfun, log, err));

  td.detach();

}
*/

