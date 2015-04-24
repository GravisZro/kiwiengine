#ifndef LUA_SANDBOX_H
#define LUA_SANDBOX_H

#include <soloutils>

#include <QCoreApplication>
#include <QThread>

struct lua_State;

class CLuaSandbox : public QThread
{
public:
  CLuaSandbox(QObject* parent = nullptr);
  ~CLuaSandbox(void);

  CLuaSandbox& setArguments(const HUString module, const list_t<KVPair>& args);
  CLuaSandbox& setArguments(const HUString module, const list_t<pair_t<QVariant, QVariant>>& args);

  HUString execScript(void);

private:
  void run(void);

  lua_State* L; // lua instance
  HUString m_rval;
  HUString m_module;
  list_t<pair_t<QVariant, QVariant>> m_args;
};

#endif // LUA_SANDBOX_H
