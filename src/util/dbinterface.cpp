#include "dbinterface.h"

#ifndef QUOTE
#define QUOTE2(x) #x
#define QUOTE(x) QUOTE2(x)
#endif

QueryThread::QueryThread(QObject* parent)
  : QThread(parent)
{  
  if(QSqlDatabase::contains(QUOTE(DB_HOSTNAME)))
    m_db = QSqlDatabase::database(QUOTE(DB_HOSTNAME));
  else
  {
    m_db = QSqlDatabase::addDatabase(QUOTE(DB_TYPE), QUOTE(DB_HOSTNAME));

    m_db.setDatabaseName(QUOTE(DB_DBNAME));
    m_db.setHostName(QUOTE(DB_HOSTNAME));
    m_db.setUserName(QUOTE(DB_USERNAME));
    m_db.setPassword(QUOTE(DB_PASSWORD));
    m_db.setPort(DB_PORT);
  }

  if(m_db.driverName() == "QMYSQL")
    m_db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

  if(!m_db.driver()->hasFeature(QSqlDriver::QuerySize))
    throw("FATAL: QSqlDriver::QuerySize not supported");

  m_query = QSqlQuery(m_db);

  connect(this, SIGNAL(finished()), this, SLOT(queryFinished()));
}

void QueryThread::run(void)
{
  try
  {
    if(!m_db.isOpen() && !m_db.open())
      throw("FATAL: failed to open database");

    if(!m_query.exec() && m_db.lastError().isValid())
    {
      qDebug() << m_db.lastError().text();
      qDebug() << m_db.lastError().driverText();
    }
  }
  catch(const char* error_message)
  {
    qDebug() << error_message;
    QSqlError err = m_db.lastError();
    qDebug() << err.text();
    qDebug() << err.driverText();

    if(HUString(error_message).startsWith("FATAL"))
      qApp->quit();
  }
}

vector_t<vector_t<QVariant>> QueryThread::parseResults(void)
{
  vector_t<vector_t<QVariant>> results;
  vector_t<QVariant> row;

  results.reserve(m_query.size());
  QSqlRecord record;
  while(m_query.next())
  {
    record = m_query.record();
    row.clear();
    row.reserve(record.count());
    for(int i = 0; i < record.count(); ++i)
      row.push_back(record.value(i));
    results.push_back(row);
  }
  return results;
}

void QueryThread::queryFinished(void)
{
  emit queryResult(parseResults());
}
