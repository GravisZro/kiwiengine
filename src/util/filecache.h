#ifndef FILECACHE_H
#define FILECACHE_H

#include <util/wikidbinterface.h>

class CFileCache : public QueryThread
{
public:
  static void    remove(quint64 version);
  static void       set(quint64 version, QByteArray data);
  static QByteArray get(quint64 version);

private:
  CFileCache(void) { }
  static CFileCache* s_instance;
  QHash<quint64, QByteArray> m_filedata;
};

#endif // FILECACHE_H
