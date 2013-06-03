#include <stdio.h>

#include <unistd.h>

#include "ConfigId.h"
#include "LuaServ.h"
#include "LuaPool.h"
using namespace std;

using namespace lua::manager;
void logfun(bool isok, const list<string> &log, const string &err)
{
  if (isok) {
    for (list<string>::const_iterator it = log.begin(); it != log.end(); ++it) {
      printf("logfun:%s\n", it->c_str());
    }
  } else {
    printf("logfun error:%s\n", err.c_str());
  }

}



void test_argcheck(lua_State *L, int index)
{
  luaL_argcheck(L, 0, index, "Fuck Fuck");
}

int test_string_check(lua_State *L)
{
  const char *s = luaL_checkstring(L, 1);

  printf("arg check string is:%s\n", s);
  return 0;
}


int test_string_cast(lua_State *L)
{
   printf("arg type is:%s\n", lua_typename(L, lua_type(L, 1)));
  if (!lua_isstring(L, 1)) {
    printf("arg is not string is:%s\n", lua_typename(L, lua_type(L, 1)));
  }

  if (!lua_isnumber(L, 1)) {
    printf("arg is not number is:%s\n", lua_typename(L, lua_type(L, 1)));
  }


  printf("arg is cast string is:%s\n", lua_tostring(L, 1));
  return 0;
}

int test_map(lua_State *L)
{
  string err;
  map<string, string> out;
  printf("top=%d\n", lua_gettop(L));
  if (!luaE_trytablemap(L, 1, out, err)) {
    return luaE_errreturn(L, err.c_str());
  }
  
  for (map<string, string>::const_iterator it = out.begin(); it != out.end(); ++it) {
    printf("%s : %s\n", it->first.c_str(), it->second.c_str());
  }

  lua_pushboolean(L, 1);
  return 1;

}

int just_fuck(lua_State *L)
{
  double d = luaL_checknumber(L, 1);

  lua_pushstring(L, "ddd");
  luaL_checknumber(L, -1);

  test_argcheck(L, 1);

  printf("lua api:just_fuck %lf\n", d);
  return 0;

}

int table_return_test(lua_State *L)
{
  lua_newtable(L);
  lua_newtable(L);
  luaE_pushfieldint(L, "aa", 123);
  luaE_pushfieldtable(L, "t0");
  luaE_pushfieldboolean(L, "t1", true);

  return 1;


}

int int64_test(lua_State *L)
{
  printf("long size:%lu\n", sizeof(long));
  printf("int64_t size:%lu\n", sizeof(int64_t));

  printf("is 64:%d\n", luaE_isint64(L, 1));

  long a = luaE_checkint64(L, 1);
  printf("int64 test:%ld\n", a);

  return 0;
}

struct stest {
  int a;
  int64_t b;
  string c;
  bool d;
};

int struct_try_test(lua_State *L)
{
  stest tt;
  string err;
  bool ok;
  ok = luaE_tryfieldint(L, 1, "a", tt.a, err);
  printf("ok=%d stack size=%d, err=%s\n", ok, lua_gettop(L), err.c_str());

  err.clear();
  ok = luaE_tryfieldint64(L, 1, "b", tt.b, err);
  printf("ok=%d stack size=%d, err=%s\n", ok, lua_gettop(L), err.c_str());

  err.clear();
  ok = luaE_tryfieldstring(L, 1, "c", tt.c, err);
  printf("ok=%d stack size=%d, err=%s\n", ok, lua_gettop(L), err.c_str());

  err.clear();
  ok = luaE_tryfieldboolean(L, 1, "d", tt.d, err);
  printf("ok=%d stack size=%d, err=%s\n", ok, lua_gettop(L), err.c_str());

  printf("a=%d b=%ld c=%s d=%d\n", tt.a, tt.b, tt.c.c_str(), tt.d);

  err.clear();

  ok = luaE_tryfieldtable(L, 1, "e", err);
  printf("ok=%d stack size=%d, err=%s\n", ok, lua_gettop(L), err.c_str());


  return 0;

}

int struct_test(lua_State *L)
{
  stest tt;
  //luaM_FIELD2STRUCT(L, tt, int, a, .)
  tt.a = luaE_checkfieldint(L, 1, "a");
  printf("stack size=%d\n", lua_gettop(L));
  tt.b = luaE_checkfieldint64(L, 1, "b");
  printf("stack size=%d\n", lua_gettop(L));
  luaE_checkfieldstring(L, 1, "c", tt.c);
  printf("stack size=%d\n", lua_gettop(L));
  tt.d = luaE_checkfieldboolean(L, 1, "d");
  printf("stack size=%d\n", lua_gettop(L));

  printf("a=%d b=%ld c=%s d=%d\n", tt.a, tt.b, tt.c.c_str(), tt.d);
  return 0;

}

static const struct luaL_Reg REG_FUNS_LIST0[] = {
  {"just_fuck", just_fuck},
  {"int64_test", int64_test},
  {"struct_test", struct_test},

  {"table_return_test", table_return_test},
  {"struct_try_test", struct_try_test},
  {"test_map", test_map},

  {"test_string_cast", test_string_cast},
  {"test_string_check", test_string_check},

  {NULL, NULL},
};

static const struct luaL_Reg *REG_FUNS_LIST[] = {
  REG_FUNS_LIST0,
  NULL,
};

const char *REG_FUNS_NAME[] = {
  "justtest",
  NULL,
};

int main()
{

  ConfigId cfg("test",
               "/data/home/guangxiang.feng/lua_test/b5.1/nb/luaManager/servconf/",
               //"/home/helen/newnb/nb/luaManager/servconf/",
               "script_conf.lua",
               "serv_conf.lua",
               "flush_conf.lua"
               );

  cfg.start(logfun);


  string err;
  printf("-------------------------------------------\n");
  printf("==========channel 0\n");
  // ---luapool test
  LuaPool lp0(
             1, 10,
	     cfg
             );


  if (!lp0.init(REG_FUNS_NAME, REG_FUNS_LIST, NULL, NULL, err)) {
    printf("lua pool init error:%s\n", err.c_str());
    //return 0;
  }

  boost::shared_ptr< LuaPool::LuaHandler > handler = lp0.get_handler(0);
  printf("pool size: %d\n", lp0.size());
  LuaServ *sv0 = handler->serv();
  if (NULL == sv0) {
    printf("get handler error!\n");
    return 1;
  }
  lua_State *L = NULL;

  L = sv0->stack_fun("FT_1", "testint64", err);
  luaE_pushint64(L, 8599);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    printf("stack_fun error pcall %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    return 1;
  }




  sleep(300);
  for (;;) {

    /*
  printf("====call test int64 now====\n");
  luaE_pushint64(L, 8599);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    printf("stack_fun error pcall %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    return 1;
  }
    */
  sleep(5);

  }


  return 0;
  printf("==========channel 1\n");
  LuaPool lp1(
             2, 5,
	     cfg
             );

  if (!lp1.init(NULL, NULL, NULL, NULL, err)) {
    printf("lua pool init error:%s\n", err.c_str());
    //return 0;
  }



  return 0;
  //----------------------------------

  LuaServ sv(cfg);


  if (!sv.load(err)) {
    printf("%s\n", err.c_str());
  }

  L = sv.stack_fun("FT_1", "test", err);
  sv.state().stackDump("test stack 0:");
  if (NULL == L) {
    printf("stack_fun err:%s\n", err.c_str());
    return 0;
  }


  lua_pushstring(L, "fuck you");
  sv.state().stackDump("test stack 1:");
  if (lua_pcall(L, 1, 1, 0) != 0) {
    sv.state().stackDump("test stack 2:");
    printf("stack_fun err pcall:%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    sv.state().stackDump("test stack 2.1:");
    return 0;

  }

  string sinfo = lua_tostring(L, -1);
  printf("stack_fun info:%s\n", sinfo.c_str());
  lua_pop(L, 1);
  sv.state().stackDump("test stack 3:");


  // -----------------test stack var

  L = sv.stack_var("FT_1", "testvar", err);
  sv.state().stackDump("test stack var 0:");
  if (NULL == L) {
    printf("stack_var err:%s\n", err.c_str());
    sv.state().stackDump("test stack var 1:");
    return 0;
  }


  sv.state().stackDump("test stack var 2:");

  sleep(120);

  return 0;


}

