#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <soloutils>
#include <QtSql>
#include <memory>

class QueryThread : public QThread
{
  Q_OBJECT
public:
  QueryThread(QObject* parent = nullptr);

  void run(void);

  QSqlQuery getQuery(void) const { return m_query; }
  template<typename... Args> void                                 setQuery  (const HUString& query_str, Args... args);
  template<typename... Args> void                                 asyncQuery(const HUString& query_str, Args... args);
  template<typename... Args> static vector_t<vector_t<QVariant>>  fastQuery (const HUString& query_str, Args... args); // syncronous query

signals:
  void queryResult(vector_t<vector_t<QVariant>> result);

private slots:
  void queryFinished(void);

private:
  vector_t<vector_t<QVariant>> parseResults(void);

  QSqlDatabase m_db;
  QSqlQuery m_query;
};

template<typename... Args>
void QueryThread::setQuery(const HUString& query_str, Args... args)
{
  vector_t<QVariant> values = {args...};
  m_query.prepare(query_str);
  for(auto pos = values.begin(); pos != values.end(); ++pos)
    m_query.addBindValue(*pos);
}

template<typename... Args>
vector_t<vector_t<QVariant>> QueryThread::fastQuery(const HUString& query_str, Args... args)
{
  QueryThread db;
  db.setQuery(query_str, args...);
  db.run();
  return db.parseResults();
}

template<typename... Args>
void QueryThread::asyncQuery(const HUString& query_str, Args... args)
{
  setQuery(query_str, args...);
  start();
}

#endif // DBINTERFACE_H
