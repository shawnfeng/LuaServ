#ifndef __LUA_MANAGER_LUAPOOL__
#define __LUA_MANAGER_LUAPOOL__

#include <vector>

#include "LuaServ.h"

namespace lua {
namespace manager {




class LuaPool {
 public:
  //------------------------
class LuaHandler {
  int type_;
  LuaPool *pool_;
  LuaServ *ls_;
 public:

  LuaServ *serv() {return ls_;}

 LuaHandler(int type, LuaPool *pool)
   : type_(type), pool_(pool), ls_(NULL)
  {
    ls_ = pool_->alloc(type_);
  }


  ~LuaHandler() {
    if (ls_ != NULL) {
      pool_->free(type_, ls_);
      ls_ = NULL;
    }
  }

};
  //------------------------
 private:
  int pool_size_;
  int jobs_sum_;
  int *job_count_;
  ConfigId &cfg_;

  std::vector<LuaServ *> pool_;


  // ---------------------------
  typedef boost::mutex::scoped_lock ScopedLock;
  boost::mutex mutex_;


 private:

  //void check_config(void (*logfun)(bool, const std::list<std::string> &, const std::string &), std::list<std::string> &log, std::string &err);

 public:
  ~LuaPool() {
    if (job_count_) {
      delete [] job_count_;
    }

    for (std::vector<LuaServ *>::iterator it = pool_.begin(); it != pool_.end(); ++it) {
      if (*it != NULL) {
	delete *it;
	*it = NULL;
      }
    }

  }
  LuaPool(
          int pool_size,
          int jobs_sum,
	  ConfigId &cfg
          ) : pool_size_(pool_size), jobs_sum_(jobs_sum),
    job_count_(NULL),
    cfg_(cfg) {

    job_count_ = new int[jobs_sum];
    for (int i = 0; i < jobs_sum; ++i) {
      job_count_[i] = 0;

    }

    for (int i = 0; i < pool_size; ++i) {
      LuaServ *p = new LuaServ(cfg_);
      pool_.push_back(p);
    }


  }

  LuaServ *alloc(int type);
  void free(int type, LuaServ *lua);
  bool init(
            const char **lib_name, const struct luaL_Reg **fun_list,
            void (*cb)(lua_State *, void *), void *arg,
            std::string &err
            );

  int size() { ScopedLock lock(mutex_); return (int)pool_.size(); }

  //void start(void (*logfun)(bool, const std::list<std::string> &, const std::string &));


  // -------------------
  boost::shared_ptr< LuaHandler > get_handler(int type) {
		return boost::shared_ptr< LuaHandler >(new LuaHandler(type, this));
	}

  

};


};
};


#endif
