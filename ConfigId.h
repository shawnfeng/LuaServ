#ifndef __LUA_MANAGER_CONFIGID__
#define __LUA_MANAGER_CONFIGID__

#include <map>
#include <string>
#include <set>
#include <list>

#include <boost/thread.hpp>  
#include <boost/thread/mutex.hpp>
#include "LuaState.h"

namespace lua {
namespace manager {

class FileInfo {
  int stamp_; // 文件修改时间
  std::string sha1_; // 上次更新时候文件对sha1
  std::string path_;

 public:
 FileInfo() : stamp_(0) {}
  int stamp() const { return stamp_; }
  const std::string &sha1() const { return sha1_; }
  const std::string &path() const { return path_; }

  void set_stamp(int st) { stamp_ = st; }
  void set_sha1(const char *sha1) { sha1_ = sha1; }
  void set_path(const char *path) { path_ = path; }

};


class ConfigId {
 enum {
   ONE_CHECK_INTV = 30,

   DEFAULT_SERV_CHECK = 60,

   IS_DEBUG = 0,
 };


  std::string serv_name_;
  std::string root_path_; // 配置所在根路径
  std::string idcfg_path_;
  std::string svcfg_path_;
  std::string fscfg_path_;
  // ---------------------------
  FileInfo idcfg_;
  FileInfo svcfg_;
  FileInfo fscfg_;

  std::map<int, std::string> id2path_;
  std::set<int> servids_;
  int flush_;

  // ---------------------------
  int check_stamp_;
  std::map<int, FileInfo> id2info_; // 用于更新检查
  // ---------------------------
  int upflag_; // 存储更新的时间戳，用于更新标识，如果该配置变更了，表示需要更新状态

  // ---------------------------
  typedef boost::shared_lock< boost::shared_mutex > ReadLock;
  typedef boost::unique_lock< boost::shared_mutex > WriteLock;

  mutable boost::shared_mutex rwmutex_; 
  
 private:

  bool load_serv(const char *path, std::string &err);
  bool load_flush(const char *path, std::string &err);
  bool load_id(const char *path, std::string &err);
  bool check_stamp(const char *path, int &stamp);
  bool check_sha1(const char *path, std::string &sha1);
  bool check_load(const char *path,
		  bool (ConfigId:: *load_fun)(const char *, std::string &),
		  FileInfo &ifo,
		  std::list<std::string> &log,
		  std::string &err);
  bool cmp_info(const char *path,
                FileInfo &ifo,
                bool &isup, // is file sha1 diff
                bool &ischg, // file info is change
		std::list<std::string> &log,
                std::string &err);

  bool check_sv2id(std::string &err);
  bool check_idinfo(bool &isup, std::list<std::string> &log, std::string &err);
  void gen_log(int be_st, int af_st,
	       const char *be_sha1, const char *af_sha1,
	       const char *be_path, const char *af_path,
	       std::list<std::string> &log
	       );


  void check_config(void (*logfun)(bool, const std::list<std::string> &, const std::string &), std::list<std::string> &log, std::string &err);


  
 public:
 ConfigId(const char *serv_name, const char *rt, const char *id_cfg, const char *sv_cfg, const char *fs_cfg)
   : serv_name_(serv_name),
    root_path_(rt), idcfg_path_(root_path_+id_cfg), 
    svcfg_path_(root_path_+sv_cfg),
    fscfg_path_(root_path_+fs_cfg),
    flush_(DEFAULT_SERV_CHECK),
    check_stamp_(0),
    upflag_(0) {}

  std::map<int, std::string> id_info() { return id2path_; }



  void get_serv_script(std::map<int, std::pair<std::string, std::string> > &info) const;
  void root_path(std::string &root) { root = root_path_; }
  int upflag() const { return upflag_; }
  int get_check_intv() const { return flush_; };
  // 检查文件更改
  bool check_change(std::list<std::string> &log, std::string &err);
  
  void start(void (*logfun)(bool, const std::list<std::string> &, const std::string &));

};

};
};


#endif
