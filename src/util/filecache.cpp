#include "filecache.h"

#include <QtSql>

CFileCache* CFileCache::s_instance = new CFileCache();

void CFileCache::remove(quint64 version)
{
  s_instance->setQuery("DELETE FROM filedata WHERE version = :VERSION", version);
  s_instance->run();

  if(s_instance->m_filedata.contains(version)) // if cached
    s_instance->m_filedata.remove(version);    // remove from cache
}

void CFileCache::set(quint64 version, QByteArray data)
{
  s_instance->setQuery("INSERT INTO filedata (version, data) VALUES (:VERSION, :DATA )", data.toBase64(), version);
  s_instance->run();

  if(s_instance->m_filedata.contains(version)) // if cached
    s_instance->m_filedata.remove(version);    // remove from cache
}

QByteArray CFileCache::get(quint64 version)
{
  if(s_instance->m_filedata.contains(version)) // if cached
    return s_instance->m_filedata[version];    // return cached data

  s_instance->setQuery("SELECT data FROM filedata WHERE version = :VERSION", version);
  s_instance->run();
  QSqlQuery query = s_instance->getQuery();
  query.next();
  return s_instance->m_filedata[version] = query.value(0).toByteArray();
}
