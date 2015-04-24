#include "lua_sandbox.h"

#include <util/wikidbinterface.h>
#include <globalvars.h>

#include <list>
#include <cassert>
#include <lua.hpp>
#include <htmlgen/encode.h>

static inline void lua_push(lua_State* L, const bool& var)
  { lua_pushboolean(L, var ? 1 : 0); }

static inline void lua_push(lua_State* L, const unsigned int& var)
  { lua_pushunsigned(L, var); }

static inline void lua_push(lua_State* L, const int& var)
  { lua_pushinteger(L, var); }

static inline void lua_push(lua_State* L, const double& var)
  { lua_pushnumber(L, var); }

static inline void lua_push(lua_State* L, const char* var)
  { lua_pushstring(L, var); }

static inline void lua_push(lua_State* L)
  { lua_pushnil(L); }


static void lua_push(lua_State* L, QVariant var)
{
  //qDebug() << "pushing:" << var.toString();

  switch(var.type())
  {
    default: lua_pushnil(L); break;
    case QVariant::Bool   : lua_push(L, var.toBool  ()); break;
    case QVariant::UInt   : lua_push(L, var.toUInt  ()); break;
    case QVariant::Int    : lua_push(L, var.toInt   ()); break;
    case QVariant::Double : lua_push(L, var.toDouble()); break;

    case QVariant::ByteArray:
      {{
        QByteArray arr = var.toByteArray();
        lua_pushlstring(L, arr.data(), arr.size());
      }}
      break;

    case QVariant::Url:
    case QVariant::String:
      {{
        QByteArray arr = var.toString().toUtf8();
        lua_pushlstring(L, arr.data(), arr.size());
      }}
      break;
  }
}


template<int mem_limit>
void* alloc_for_lua(void*, void* p, size_t o, size_t n)
{
  static int mem_used = 0;
  mem_used += n - o;
  assert(mem_used < mem_limit * 1024);
  return realloc(p,n);
}

static int db_ll_require (lua_State *L)
{
  const char *name = luaL_checkstring(L, 1);
  //qDebug() << "require:" << name;
  lua_settop(L, 1); // _LOADED table will be at index 2
  lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
  lua_getfield(L, 2, name); // _LOADED[name]
  if (!lua_toboolean(L, -1)) // if package is NOT already loaded
  {
    lua_pop(L, 1); // remove 'getfield' result

    if(luaL_dostring(L, latest_page(ENamespaces::eModule, name)))
    {
      qDebug() << lua_tostring(L, -1);
      lua_pop(L, 1); // remove error string
      lua_pushnil(L); // set return value to nil
    }

    if (!lua_isnil(L, -1)) // non-nil return?
      lua_setfield(L, 2, name); // _LOADED[name] = returned value
    lua_getfield(L, 2, name);
    if (lua_isnil(L, -1)) // module did not set a value?
    {
      lua_pushboolean(L, 1); // use true as result
      lua_pushvalue(L, -1); // extra copy to be returned
      lua_setfield(L, 2, name); // _LOADED[name] = true
    }
  }
  return 1;
}
/*
#include <QUrl>
static int url_encode (lua_State *L)
{
  const char *url = luaL_checkstring(L, 1);
  lua_push(L, QString(QUrl::toPercentEncoding(QString(url))));
  return 1;
}
*/

static int image_properties(lua_State *L)
{
  const char *name = luaL_checkstring(L, 1);
  vector_t<QVariant> data = media_info(name);

  lua_newtable(L);

  lua_push(L, "exists");
  lua_push(L, !data.empty());
  lua_settable(L, -3);

  if(!data.empty())
  {
    lua_push(L, "width");
    if(data[1].toInt() > 0)
      lua_push(L, data[1].toInt());
    else
      lua_push(L);
    lua_settable(L, -3);

    lua_push(L, "height");
    if(data[2].toInt() > 0)
      lua_push(L, data[2].toInt());
    else
      lua_push(L);
    lua_settable(L, -3);
  }

  return 1;
}


static int wiki_url_encode(lua_State *L)
{
  const char* text = luaL_checkstring(L, 1);
  lua_push(L, encoder(text).operator const char *());
  return 1;
}

CLuaSandbox::CLuaSandbox(QObject* parent)
  : QThread(parent)
{
  static const std::list<luaL_Reg> lua_libs =
  {
    {"_G", luaopen_base},
    {LUA_MATHLIBNAME, luaopen_math  },
    {LUA_TABLIBNAME , luaopen_table },
    {LUA_STRLIBNAME , luaopen_string},
  };

  L = lua_newstate(alloc_for_lua<128>, nullptr);

  for (auto lib : lua_libs)
  {
    luaL_requiref(L, lib.name, lib.func, 1);
    lua_pop(L, 1);
  }

  luaL_getsubtable(L, LUA_REGISTRYINDEX, "_PRELOAD");

  for (auto lib : lua_libs)
  {
    lua_pushcfunction(L, lib.func);
    lua_setfield(L, -2, lib.name);
  }

  lua_pop(L, 1);

  lua_pushcfunction(L, db_ll_require);
  lua_setglobal(L, "require");

  lua_pushcfunction(L, wiki_url_encode);
  lua_setglobal(L, "wiki_url_encode");

  lua_pushcfunction(L, image_properties);
  lua_setglobal(L, "image_properties");

  lua_pushinteger(L, wgThumbLimits[wgDefaultUserOptions.value("thumbsize").toInt()]);
  lua_setglobal(L, "thumbnail_size");
}

CLuaSandbox::~CLuaSandbox(void)
{
  lua_close(L);
  L = nullptr;
}

CLuaSandbox& CLuaSandbox::setArguments(const HUString module, const list_t<KVPair>& args)
{
  m_module = module;
  for(auto pos = args.begin(); pos != args.end(); ++pos)
  {
    m_args.emplace_back(pos->key  ().isEmpty() ? QVariant() : pos->key  (),
                        pos->value().isEmpty() ? QVariant() : pos->value());
  }
  return *this;
}

CLuaSandbox& CLuaSandbox::setArguments(const HUString module, const list_t<pair_t<QVariant, QVariant>>& args)
{
  m_module = module;
  m_args = args;
  return *this;
}

HUString CLuaSandbox::execScript(void)
{
/*
  qDebug() << "calling module:" << m_module;
  qDebug() << "arguments:";
  for(const auto& arg : m_args)
    qDebug() << "\tname:" << arg.key() << "\tvalue:" << arg.value();
*/
  if(parent() != qApp) // if already in a seperate thread
    run(); // run synchronously
  else
  {
    start();
    wait(500); // run for a max of 500 miliseconds
    if(isRunning())
    {
      terminate();
      m_rval = "Script exceeded maximum allowed execution time";
    }
  }

  if(m_rval.isEmpty())
  {
    m_rval = "\nMODULE FAILURE!"
             "\ncalled module: "
           + m_module
           + "\narguments:";
    for(const auto& arg : m_args)
      m_rval += "\n"
                "\tname: "
              + arg.key().toString()
              + "\tvalue: "
              + arg.value().toString();
  }
//  qDebug() << "lua script return value:" << m_rval;
  return m_rval;
}

void CLuaSandbox::run(void)
{
  HUString script;
  m_rval.clear();

  if(page_exists(ENamespaces::eModule, m_module))
    script = latest_page(ENamespaces::eModule, m_module);

  if(script.isNull())
    m_rval = "Module \""+ m_module +"\" does not exist";
  else
  {
    try
    {
      if(luaL_dostring(L, script))
        throw(lua_tostring(L, -1));

      auto func = m_args.front();

      if(func.key() != "function")
        throw("first argument is not a function name");

      lua_getfield(L, lua_gettop(L), HUString(func.value()));
      lua_newtable(L);

      m_args.pop_front();
      unsigned int i = 0;
      for(const auto& arg : m_args)
      {
        ++i;
        if(arg.key().isNull())
          lua_push(L, i);
        else
          lua_push(L, arg.key());

        lua_push(L, arg.value());
        lua_settable(L, -3);
      }

      if(lua_pcall(L, 1, 1, 0))
        throw(lua_tostring(L, -1));

      if(!lua_isstring(L, -1))
        throw("result is not a string");

      m_rval = lua_tostring(L, -1);
    }
    catch(const char* err)
    {
      m_rval = HUString("error: ") + err;
      qDebug() << m_rval;
    }
  }
}
