#include <sys/stat.h> 

#include <openssl/sha.h>

#include <iostream>
#include "ConfigId.h"
using namespace std;
using namespace lua::manager;

static const char *script_id = "id";
static const char *script_path = "path";



bool ConfigId::check_load(const char *path,
			  bool (ConfigId:: *load_fun)(const char *, std::string &),
			  FileInfo &ifo,
			  list<string> &log,
			  std::string &err)
{
  int stamp = 0;
  string sha1;
  bool chpath = false;

  if (!check_stamp(path, stamp)) {
    err = path;
    err += " check stamp error!";
    return false;
  }

  if (ifo.path() != path) chpath = true;



  if (chpath || stamp != ifo.stamp()) {
    if (!check_sha1(path, sha1)) {
      err = path;
      err += " check sha1 error!";
      return false;
    }

    gen_log(ifo.stamp(), stamp,
	    ifo.sha1().c_str(), sha1.c_str(),
	    ifo.path().c_str(), path,
	    log);



    if (sha1 != ifo.sha1()) {
      if (!(this->*load_fun)(path, err)) {
        return false;
      }
      // 没有错误时候在改
      ifo.set_sha1(sha1.c_str());
    }
    if (chpath) ifo.set_path(path);
    if (stamp != ifo.stamp()) ifo.set_stamp(stamp);
    
  }
  //  printf("check_load:stamp=%d path=%s Bsha1=%s Asha1=%s\n", stamp, path, ifo.sha1().c_str(), sha1.c_str());


  return true;
}


void ConfigId::gen_log(int be_st, int af_st,
		       const char *be_sha1, const char *af_sha1,
		       const char *be_path, const char *af_path,
		       list<string> &log
		       )
{

  char buff[500] = {0};

  snprintf(buff, sizeof(buff) - 1, "FILE-->%d:%d %s:%s %s:%s",
	   be_st, af_st,
	   be_sha1, af_sha1,
	   be_path, af_path
	   );

  log.push_back(buff);

}


bool ConfigId::cmp_info(const char *path,
                        FileInfo &ifo,
                        bool &isup, // is file sha1 diff
                        bool &ischg, // file info is change
			list<string> &log,
                        std::string &err)
{
  int stamp = 0;
  string sha1;
  bool chpath = false;

  if (!check_stamp(path, stamp)) {
    err = path;
    err += " check stamp error!";
    return false;
  }


  if (ifo.path() != path) chpath = true;

  //printf("cmp_info:B stamp=%d path=%s A stamp=%d path=%s\n", ifo.stamp(), ifo.path().c_str(), stamp, path);

  if (chpath || stamp != ifo.stamp()) {
    if (!check_sha1(path, sha1)) {
      err = path;
      err += " check sha1 error!";
      return false;
    }

    //printf("cmp_info:B sha1=%s A sha1=%s\n", ifo.sha1().c_str(), sha1.c_str());
    gen_log(ifo.stamp(), stamp,
	    ifo.sha1().c_str(), sha1.c_str(),
	    ifo.path().c_str(), path,
	    log);



    if (sha1 != ifo.sha1()) {
      ifo.set_sha1(sha1.c_str());
      isup = true;
    }

    if (chpath) {
      ifo.set_path(path);
    }

    if (stamp != ifo.stamp()) {
      ifo.set_stamp(stamp);
    }
    ischg = true;
    
  }

  //printf("stamp=%d, path=%s, sha1=%s, ischg=%d, isup=%d\n", ifo.stamp(), ifo.path().c_str(), ifo.sha1().c_str(), ischg, isup);

  return true;
}

bool ConfigId::check_sv2id(string &err)
{
  const set<int> &ids = servids_;
  for (set<int>::const_iterator jt = ids.begin(); jt != ids.end(); ++jt) {
    if (id2path_.find(*jt) == id2path_.end()) {
      err = "some servive id cannot find!";
      return false;
    }
  }

  return true;
}


bool ConfigId::check_idinfo(bool &isup, list<string> &log, string &err)
{
  bool ischg = false;
  map<int, FileInfo> tmp_idinfos;
  {
    ReadLock rlock(rwmutex_);
    const set<int> &ids = servids_;
    for (set<int>::const_iterator jt = ids.begin(); jt != ids.end(); ++jt) {
      map<int, string>::const_iterator kt = id2path_.find(*jt);
      if (kt == id2path_.end()) {
        err = "some servive id cannot find!";
        return false;
      }

      const string &path = root_path_+kt->second;

      FileInfo tmp;
      map<int, FileInfo>::const_iterator yt = id2info_.find(kt->first);
      if (yt != id2info_.end()) tmp = yt->second;

      if (!cmp_info(path.c_str(), tmp, isup, ischg, log, err)) {
        return false;
      }


      tmp_idinfos[kt->first] = tmp;


    }
  }
  // need whrite lock
  if (ischg) {
    WriteLock wlock(rwmutex_);
    id2info_.swap(tmp_idinfos);
  }

  return true;

}

bool ConfigId::check_change(list<string> &log, string &err)
{
  if (IS_DEBUG) {
    printf("###### ConfigId::check_change do\n");
  }

  if (!check_load(fscfg_path_.c_str(), &ConfigId::load_flush, fscfg_, log, err)) {
    return false;
  }

  // 没有加载过 || 已经大于检测时间
  int stamp = time(NULL);
  int bef_stamp = check_stamp_;
  int intv = get_check_intv();
  //printf("check_chage: bef=%d, now=%d, now-bef=%d intv=%d\n", bef_stamp, stamp, stamp - bef_stamp, intv);
  if (bef_stamp == 0 || stamp - bef_stamp > intv) {
    check_stamp_ = stamp;
    if (!check_load(svcfg_path_.c_str(), &ConfigId::load_serv, svcfg_, log, err)) {
      return false;
    }

    if (!check_load(idcfg_path_.c_str(), &ConfigId::load_id, idcfg_, log, err)) {
      return false;
    }

    if (!check_sv2id(err)) {
      return false;
    }

    bool isup = false;
    if (!check_idinfo(isup, log, err)) {
      return false;
    }

    // have changed file, update flag
    if (isup) {
      upflag_ = stamp;
    }

  }

  if (IS_DEBUG) {
    printf("###### ConfigId::check_change over\n");
  }


  return true;
}



bool ConfigId::load_id(const char *path, string &err)
{
  LuaState state;
  map<int, string> i2p;
  const char *sset_fun = "get_script_set";  
  if (!state.load_file(path, err) || !state.pcall(err))
    return false;

  lua_State *L = state.st();

  lua_getglobal(L, sset_fun);
  if (!lua_isfunction(L, -1)) {
    err = sset_fun;
    err += " is not a function";
    return false;
  }

  if (lua_pcall(L, 0, 1, 0) != 0) {
    err = sset_fun;
    err += " call get script set function error:";
    err += lua_tostring(L, -1);
    return false;
  }

  if (!lua_istable(L, -1)) {
    err = sset_fun;
    err += " return is not a table";
    return false;

  }

  int n = lua_objlen(L, -1);
  if (IS_DEBUG) {
    printf("###### script set len=%d\n", n);
  }

  for (int i = 1; i <=n; i++) {
    // -1 table在栈中的位置 i为数据索引
    lua_rawgeti(L, -1, i);
    if (!lua_istable(L, -1)) {
      err = "invalid config table";
      return false;
    }
    lua_getfield(L, -1, script_id);
    if (!lua_isnumber(L, -1)) {
      err = "invalid config id";
      return false;
    }

    int sid = (int)lua_tonumber(L, -1);
    lua_pop(L, 1); // 从栈中弹出一个元素

    lua_getfield(L, -1, script_path);
    if (!lua_isstring(L, -1)) {
      err = "invalid config path";
      return false;
    }

    string spath = lua_tostring(L, -1);
    lua_pop(L, 1);

    i2p[sid] = spath;
    lua_pop(L, 1); // 弹出放到顶部的配置项
  }

  id2path_.swap(i2p);

  return true;

}

bool ConfigId::load_flush(const char *path, std::string &err)
{
  LuaState state;
  map<string, int> s2f;
  const char *flush_fun = "get_flush";
  if (!state.load_file(path, err) || !state.pcall(err))
    return false;

  lua_State *L = state.st();

  lua_getglobal(L, flush_fun);
  if (!lua_isfunction(L, -1)) {
    err = flush_fun;
    err += " is not a function";
    return false;
  }

  lua_pushstring(L, serv_name_.c_str());
  if (lua_pcall(L, 1, 1, 0) != 0) {
    err = flush_fun;
    err += " call get flush function error!";
    return false;
  }

  if (!lua_isnumber(L, -1)) {
    flush_ = DEFAULT_SERV_CHECK; // 默认刷新间隔
  } else {
    flush_ = lua_tointeger(L, -1);
  }
  if (IS_DEBUG) {
    printf("###### serv flush serv=%s flush=%d\n", serv_name_.c_str(), flush_);
  }

  return true;
  
}


bool ConfigId::load_serv(const char *path, std::string &err)
{
  LuaState state;
  set<int> sids;
  const char *load_serv_fun = "get_serv_set";  
  if (!state.load_file(path, err) || !state.pcall(err))
    return false;

  lua_State *L = state.st();

  lua_getglobal(L, load_serv_fun);
  if (!lua_isfunction(L, -1)) {
    err = load_serv_fun;
    err += " is not a function";
    return false;
  }

  lua_pushstring(L, serv_name_.c_str());
  if (lua_pcall(L, 1, 1, 0) != 0) {
    err = load_serv_fun;
    err += " call get serv ids load function error:";
    err += lua_tostring(L, -1);
    return false;
  }

  if (!lua_istable(L, -1)) {
    err = load_serv_fun;
    err += " return is not a table";
    return false;

  }

  int n = lua_objlen(L, -1);
  if (IS_DEBUG) {
    printf("###### serv idset len=%d\n", n);
  }
  for (int i = 1; i <=n; i++) {
    // -1 table在栈中的位置 i为数据索引
    lua_rawgeti(L, -1, i);
    if (!lua_isnumber(L, -1)) {
      err = "invalid config ";
      err += load_serv_fun;
      return false;
    }
    int sid = lua_tointeger(L, -1);
    lua_pop(L, 1); // 从栈中弹出一个元素

    // 保存
    sids.insert(sid);
  }
  servids_.swap(sids);

  return true;

}

void ConfigId::get_serv_script(map<int, pair<string, string> > &info) const
{
  ReadLock rlock(rwmutex_);
  
  for (map<int, FileInfo>::const_iterator it = id2info_.begin(); it != id2info_.end(); ++it) {
    info[it->first] = pair<string, string>(it->second.path(), it->second.sha1());
  }

}

bool ConfigId::check_stamp(const char *path, int &stamp)
{
  // 获取文件最后修改时间
  struct stat statbuf;
  if (stat(path, &statbuf)) return false;
  stamp = statbuf.st_ctime;
  return true;

}

bool ConfigId::check_sha1(const char *path, string &sha1)
{
  char buf[512];
  SHA_CTX s;
  unsigned char hash[20];

  FILE *fp = fopen(path,"rb");
  if (!fp) return false;

  SHA1_Init(&s);
  int size;
  while ((size=fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
    SHA1_Update(&s, buf, size);
  } 
  SHA1_Final(hash, &s);
  fclose(fp);

  sha1.clear();
  char tmp[10] = {0};
  for (int i=0; i < 20; i++) {
    sprintf (tmp, "%.2x", (int)hash[i]);
    sha1 += tmp;
  }

  return true;
 
}

void ConfigId::check_config(void (*logfun)(bool, const std::list<std::string> &, const std::string &), list<string> &log, string &err)
{
  for (;;) {
    if (IS_DEBUG) {
      printf("###### ConfigId::check_config check\n");
    }
    log.clear();
    bool isok = check_change(log, err);
    logfun(isok, log, err);
    sleep(ONE_CHECK_INTV);
  }
}

// 使用这个函数后，Configid内部将自提供一个检测线程来检查配置变化
// 也就是说不需要外部参与检测过程
// 但是如果，外部不想增加这一个检测线程的开销，或者有其他的检查策略，或者不想支持实时的检查等可以不调用该函数
// 才改用外部一个统一的检测线程来完成这个过程
// 线程中的检测函数调用check_change函数
// !!!!!但是无论如何，请不要开启两个检查过程
void ConfigId::start(void (*logfun)(bool, const std::list<std::string> &, const std::string &)) {
  if (IS_DEBUG) {
    printf("###### ConfigId::start call\n");
  }
  list<string> log;
  string err;
  // 为了保证数据加载首先完成，防止线程异步性造成的异步加载问题，在加载前就使用
  bool isok = check_change(log, err);
  logfun(isok, log, err);
  if (IS_DEBUG) {
    printf("###### ConfigId::start check thread\n");
  }
  boost::thread td(boost::bind(&ConfigId::check_config, this, logfun, log, err));

  td.detach();

}
