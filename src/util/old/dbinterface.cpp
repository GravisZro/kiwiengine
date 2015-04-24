#include "dbinterface.h"

#ifndef QUOTE
#define QUOTE2(x) #x
#define QUOTE(x) QUOTE2(x)
#endif

QueryThread::QueryThread(QObject* parent)
  : QThread(parent)
{  
  if(QSqlDatabase::contains("DBIO"))
    m_db = QSqlDatabase::database("DBIO");
  else
  {
    m_db = QSqlDatabase::addDatabase(QUOTE(DB_TYPE), "DBIO");

    m_db.setDatabaseName(QUOTE(DB_DBNAME));
    m_db.setHostName(QUOTE(DB_HOSTNAME));
    m_db.setUserName(QUOTE(DB_USERNAME));
    m_db.setPassword(QUOTE(DB_PASSWORD));
    m_db.setPort(DB_PORT);
  }

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

    if(!m_query.exec())
    {
      if(m_db.lastError().isValid())
      {
        qDebug() << m_db.lastError().text();
        qDebug() << m_db.lastError().driverText();
      }
    }
  }
  catch(const char* error_message)
  {
    qDebug() << error_message;
    QSqlError err = m_db.lastError();
    qDebug() << err.text();
    qDebug() << err.driverText();

    if(QString(error_message).startsWith("FATAL"))
      qApp->quit();
  }
}


void QueryThread::queryFinished(void)
{
  emit queryResult(parseResults(m_query));
}
