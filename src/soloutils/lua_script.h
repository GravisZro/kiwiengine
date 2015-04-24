#if 0
ndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "hustring.h"
#include <lua.hpp>
#include <cstddef>

class Lua
{
  lua_State* ptr;
public:
  typedef int idx_t;

  inline operator lua_State*(void) const { return ptr; }
  inline lua_State* operator =(lua_State* state) { return ptr = state; }

// state manipulation
  inline Lua(lua_State* state) { ptr = state; }
  inline Lua(lua_Alloc f, void *ud) { ptr = lua_newstate(f, ud); }
  inline ~Lua(void) { lua_close(ptr); }

  inline Lua createThread(void) const { return lua_newthread(ptr); }


  lua_CFunction atpanic(lua_CFunction panicf);

  inline double version(void) const { return *lua_version(ptr); }


// basic stack manipulation
  inline int  absIndex   (idx_t pos) const { return lua_absindex(ptr, pos); }
  inline int  getTop     (void     ) const { return lua_gettop(ptr); }
  inline void setTop     (idx_t pos) { lua_settop(ptr, pos); }
  inline void pushValue  (idx_t pos) { lua_pushvalue(ptr, pos); }
  inline void remove     (idx_t pos) { lua_remove(ptr, pos); }
  inline void insert     (idx_t pos) { lua_insert(ptr, pos); }
  inline void replace    (idx_t pos) { lua_replace(ptr, pos); }
  inline void copy       (idx_t from, idx_t to) { lua_copy(ptr, from, to); }
  inline bool checkStack (int needed) { return lua_checkstack(ptr, needed); }


  enum class Type : int
  {
    None = -1,
    Nil,
    Boolean,
    LightUserData,
    Number,
    String,
    Table,
    Function,
    UserData,
    Thread,
  };

// access functions (stack -> C)

  inline bool            isNumber    (idx_t pos) const { return lua_isnumber    (ptr, pos); }
  inline bool            isString    (idx_t pos) const { return lua_isstring    (ptr, pos); }
  inline bool            isCFunction (idx_t pos) const { return lua_iscfunction (ptr, pos); }
  inline bool            isFunction  (idx_t pos) const { return lua_isfunction  (ptr, pos); }
  inline bool            isUserdata  (idx_t pos) const { return lua_isuserdata  (ptr, pos); }
  inline int             type        (idx_t pos) const { return lua_type        (ptr, pos); }
  inline const char*     typeName    (int tp ) const { return lua_typename    (ptr, tp ); }

  inline lua_Number      toNumber    (idx_t pos, bool* ok = nullptr) { return lua_tonumberx  (ptr, pos, reinterpret_cast<int*>(ok)); }
  inline lua_Integer     toInteger   (idx_t pos, bool* ok = nullptr) { return lua_tointegerx (ptr, pos, reinterpret_cast<int*>(ok)); }
  inline lua_Unsigned    toUnsigned  (idx_t pos, bool* ok = nullptr) { return lua_tounsignedx(ptr, pos, reinterpret_cast<int*>(ok)); }
  inline bool            toBoolean   (idx_t pos) { return lua_toboolean(ptr, pos); }
  inline HUString        toString    (idx_t pos) { return lua_tolstring(ptr, pos, nullptr); }

  inline size_t          size        (idx_t pos) { return lua_rawlen       (ptr, pos); }
  inline lua_CFunction   toCFunction (idx_t pos) { return lua_tocfunction  (ptr, pos); }
  inline void*           toUserdata  (idx_t pos) { return lua_touserdata   (ptr, pos); }
  inline Lua             toThread    (idx_t pos) { return lua_tothread     (ptr, pos); }
  inline const void*     toPointer   (idx_t pos) { return lua_topointer    (ptr, pos); }


// Comparison and arithmetic functions

  enum class Arithmatic : int
  {
    Add = 0,
    Subtract,
    Multiply,
    Divide,
    Modulus,
    Exponential,
    Negate,
  };

  inline void arithmetic  (Arithmatic operation) { lua_arith(ptr, *reinterpret_cast<int*>(&operation)); }

  inline void add         (void) { arithmetic(Arithmatic::Add         ); }
  inline void subtract    (void) { arithmetic(Arithmatic::Subtract    ); }
  inline void multiply    (void) { arithmetic(Arithmatic::Multiply    ); }
  inline void divide      (void) { arithmetic(Arithmatic::Divide      ); }
  inline void modulus     (void) { arithmetic(Arithmatic::Modulus     ); }
  inline void exponential (void) { arithmetic(Arithmatic::Exponential ); }
  inline void negate      (void) { arithmetic(Arithmatic::Negate      ); }


  enum class Comparison
  {
    Equal = 0,
    LessThan,
    LessThanOrEqual,
  };


  int   rawequal(idx_t pos1, idx_t pos2);
  int   compare(idx_t pos1, idx_t pos2, int op);


  // push functions (C -> stack)
  void pushNil   (void) { lua_pushnil(ptr); }
  bool pushThread(void) { return lua_pushthread(ptr); }

  void push(double num) { lua_pushnumber(ptr, num); }
  void push(long num) { lua_pushinteger(ptr, num); }
  void push(unsigned int num) { lua_pushunsigned(ptr, num); }
  bool push(const QByteArray& data) { return lua_pushlstring(ptr, data.data(), data.size()); }
  bool push(const HUString& str) { return lua_pushstring(ptr, str.toUtf8().data()); }
  void push(lua_CFunction fn, int n = 0) { lua_pushcclosure(ptr, fn, n); }
  void push(bool val) { lua_pushboolean(ptr, val); }
  void push(void* p) { lua_pushlightuserdata(ptr, p); }

  // get functions (Lua -> stack)
  void  getglobal(const char *var);
  void  gettable(idx_t pos);
  void  getfield(idx_t pos, const char *k);
  void  rawget(idx_t pos);
  void  rawgeti(idx_t pos, int n);
  void  rawgetp(idx_t pos, const void *p);
  void  createtable(int narr, int nrec);
  void *newuserdata(size_t sz);
  int   getmetatable(int objindex);
  void  getuservalue(idx_t pos);


  // set functions (stack -> Lua)
  void  setglobal(const char *var);
  void  settable(idx_t pos);
  void  setfield(idx_t pos, const char *k);
  void  rawset(idx_t pos);
  void  rawseti(idx_t pos, int n);
  void  rawsetp(idx_t pos, const void *p);
  int   setmetatable(int objindex);
  void  setuservalue(idx_t pos);


  /*
  ** 'load' and 'call' functions (load and run Lua code)
  */
  void  callk(int nargs, int nresults, int ctx,
                             lua_CFunction k);
  #define lua_call(L,n,r)		lua_callk(L, (n), (r), 0, NULL)

  int   getctx(int *ctx);

  int   pcallk(int nargs, int nresults, int errfunc,
                              int ctx, lua_CFunction k);
  #define lua_pcall(L,n,r,f)	lua_pcallk(L, (n), (r), (f), 0, NULL)

  int   load(lua_Reader reader, void *dt,
                                          const char *chunkname,
                                          const char *mode);

  int dump(lua_Writer writer, void *data);


  /*
  ** coroutine functions
  */
  int  yieldk(int nresults, int ctx,
                             lua_CFunction k);
  #define lua_yield(L,n)		lua_yieldk(L, (n), 0, NULL)
  int  resume(lua_State *from, int narg);
  int  status();

  /*
  ** garbage-collection function and options
  */

  enum class GarbageCollection : int
  {
    Stop = 0,
    Restart,
    Collect,
    CountKBytes,
    CountBytes,
    Step,
    SetPause,
    SetStepMultiplier,
    IsRunning = 9,
    Generational,
    Incremental,
  };


  int garbage(int what, int data);


  /*
  ** miscellaneous functions
  */

  int   error();

  int   next(idx_t pos);

  void  concat(int n);
  void  len)    State *L, idx_t pos);

  lua_Alloc getallocf(void **ud);
  void      setallocf(lua_Alloc f, void *ud);



  /*
  ** ===============================================================
  ** some useful macros
  ** ===============================================================
  */

  #define lua_tonumber(L,i)	lua_tonumberx(L,i,NULL)
  #define lua_tointeger(L,i)	lua_tointegerx(L,i,NULL)
  #define lua_tounsigned(L,i)	lua_tounsignedx(L,i,NULL)

  #define lua_pop(L,n)		lua_settop(L, -(n)-1)

  #define lua_newtable(L)		lua_createtable(L, 0, 0)

  #define lua_register(L,n,f) pushcfunction(L, (f)), lua_setglobal(L, (n)))

  #define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

  #define lua_isfunction(L,n)	type(L, (n)) == LUA_TFUNCTION)
  #define lua_istable(L,n)	type(L, (n)) == LUA_TTABLE)
  #define lua_islightuserdata(L,n)	type(L, (n)) == LUA_TLIGHTUSERDATA)
  #define lua_isnil(L,n)		type(L, (n)) == LUA_TNIL)
  #define lua_isboolean(L,n)	type(L, (n)) == LUA_TBOOLEAN)
  #define lua_isthread(L,n)	type(L, (n)) == LUA_TTHREAD)
  #define lua_isnone(L,n)		type(L, (n)) == LUA_TNONE)
  #define lua_isnoneornil(L, n)	type(L, (n)) <= 0)

  #define lua_pushliteral(L, s)	\
    lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

  #define lua_pushglobaltable(L)  \
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_Rpos_GLOBALS)

  #define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)

};


#endif // LUA_SCRIPT_H
