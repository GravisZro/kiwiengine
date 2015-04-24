#include <httpio/http_daemon.h>

#include <QResource>
#include <QtCore>
#include <QtServiceController>


#include <parse/char_stream.h>
#include <netresponse/article.h>
#if 0
//#include <lua.hpp>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <stdio.h>
#endif

#include <soloutils/ustring.h>
int main(int argc, char **argv)
{
#if 0
  for(option_t opt : options)
  {
    qDebug() << "is_long      :" << opt.is_long;
    qDebug() << "flag_name    :" << QByteArray(opt.flag_name.data    , opt.flag_name.length);
    qDebug() << "value_name   :" << QByteArray(opt.value_name.data   , opt.value_name.length);
    qDebug() << "value_pattern:" << QByteArray(opt.value_pattern.data, opt.value_pattern.length);
    qDebug() << "description  :" << QByteArray(opt.description.data  , opt.description.length);
    qDebug() << "";
  }
  return 0;

#elif 0
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  const char* script_string =
      "\nlocal p = {}"
      "\n  function p.render_embedded(args)"
      "\n    local rval = \"embedded<br>start<br>\""
      "\n    for index, value in ipairs(args) do"
      "\n      rval = rval .. \"i:\" .. tostring(index) .. \" - \""
      "\n      rval = rval .. \"v:\" .. value .. \"<br>\""
      "\n    end"
      "\n    rval = rval .. \"end\""
      "\n    return rval"
      "\n  end"
      "\nreturn p";

  const char* funcname_string = "render_embedded";

  if(luaL_dostring(L, script_string))
  {
    printf("error: %s\n", lua_tostring(L, -1));
    return -1;
  }

  int idx = lua_gettop(L);

  lua_getfield(L, idx, funcname_string);

  lua_newtable(L);

  lua_pushunsigned(L, 1);
  lua_pushstring(L, "one");
  lua_settable(L, -3);

  lua_pushunsigned(L, 2);
  lua_pushstring(L, "two");
  lua_settable(L, -3);

  lua_pushunsigned(L, 3);
  lua_pushstring(L, "three");
  lua_settable(L, -3);

  lua_pushunsigned(L, 4);
  lua_pushstring(L, "four");
  lua_settable(L, -3);

  if(lua_pcall(L, 1, 1, 0))
  {
    printf("error: %s\n", lua_tostring(L, -1));
    return -1;
  };

  printf("returned type: %i\n", lua_type(L, -1));

  if(!lua_isstring(L, -1))
  {
    printf("bad type\n");
    return -1;
  }

  printf("returned value: %s\n", lua_tostring(L, -1));

  lua_close(L);

  return 0;


#elif 1
  QResource::registerResource("resources.qrc");;

  CHttpDaemon service("QtWiki Service", "A Wiki engine written in Qt/C++", KIWI_PORT, argc, argv);

  QtServiceController controller("QtWiki Service");

  if(controller.isRunning())
  {
    qDebug("\nALREADY ACTIVE!\r\n");
    return 0;
  }

  service.logMessage("\nstarting QtWiki Service\r\n", CHttpDaemon::Information);

  return service.exec();
#elif 0

  const char data[] = "{ {{ }}}";

  CCharStream stream;
  stream.setData(data, sizeof(data));

  int depth = 0, start = 0, end = 0;

  ++stream;
  ++stream;

  qDebug() << "found:" << find_match<open_curlybracket, close_curlybracket>(stream, depth, start, end);
  qDebug() << "depth:" << (int)depth;
  qDebug() << "start:" << start;
  qDebug() << "end  :" << end;

  return 0;
#elif 0
  auto results = QueryThread::fastQuery("SELECT img_timestamp, img_name, img_width, img_height FROM image ORDER BY img_name");
  for(auto pos = results.begin(); pos != results.end(); ++pos)
  {
    auto row = *pos;
//    if(row[1] != "TurboGrafx-16-Service-Manual.pdf")
      continue;
    qDebug() << row[1];
    QFile file ("/home/gravis/images/" + row[1].toString());
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    QueryThread::fastQuery("INSERT INTO image_data (img_timestamp, img_name, img_width, img_height, img_data) VALUES (?,?,?,?,?)",
                            row[0], row[1], row[2], row[3], file.readAll());
  }
  return 0;
#endif
}
