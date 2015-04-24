#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <memory>
#include <QtCore>
#include <QtSql>

class QueryThread : public QThread
{
  Q_OBJECT
public:
  QueryThread(QObject* parent = qApp);

  void run(void);

  QSqlQuery getQuery(void) const { return m_query; }
  template<typename... Args> void                        setQuery  (const QString& query_str, Args... args);
  template<typename... Args> void                        asyncQuery(const QString& query_str, Args... args);
  template<typename... Args> QVector<QVector<QVariant> > fastQuery (const QString& query_str, Args... args);


signals:
  void queryResult(QVector<QVector<QVariant> > result);

private slots:
  void queryFinished(void);
private:
  QSqlDatabase m_db;
  QSqlQuery m_query;
};


static QVector<QVector<QVariant> > parseResults(QSqlQuery query)
{
  QVector<QVector<QVariant> > results;
  QVector<QVariant> row;

  results.reserve(query.size());
  QSqlRecord record;
  while(query.next())
  {
    record = query.record();
    row.clear();
    row.reserve(record.count());
    for(int i = 0; i < record.count(); ++i)
      row.push_back(record.value(i));
    results.push_back(row);
  }
  return results;
}

template<typename... Args>
void QueryThread::setQuery(const QString& query_str, Args... args)
{
  std::vector<QVariant> values = {args...};
  m_query.prepare(query_str);
  for(auto pos = values.begin(); pos != values.end(); ++pos)
    m_query.addBindValue(*pos);
}

template<typename... Args>
QVector<QVector<QVariant> > QueryThread::fastQuery(const QString& query_str, Args... args)
{
  setQuery(query_str, args...);
  run();
  return parseResults(m_query);
}

template<typename... Args>
void QueryThread::asyncQuery(const QString& query_str, Args... args)
{
  setQuery(query_str, args...);
  start();
}

#endif // DBINTERFACE_H
