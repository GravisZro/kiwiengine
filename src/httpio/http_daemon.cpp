#include "http_daemon.h"

#include "http_server.h"
#include <netresponse/response_relay.h>
#include <netresponse/article.h>
#include <netresponse/file.h>
#include <netresponse/download.h>


CHttpDaemon::CHttpDaemon(const char* name, const char* description, const quint16 port, int& argc, char **argv)
  : QtService<QCoreApplication>(argc, argv, name),
    m_server(nullptr),
    m_port(port)
{
  setServiceDescription(description);
  setServiceFlags(QtServiceBase::CanBeSuspended);
}

CHttpDaemon::~CHttpDaemon(void)
{
}

void CHttpDaemon::start(void)
{
  if(m_server == nullptr)
  {
    m_server = new CHttpServer(m_port, application());
    qApp->installEventFilter(new CResponseRelay(qApp));
    qApp->installEventFilter(new CArticleResponse(qApp));
    qApp->installEventFilter(new CFileResponse(qApp));
    qApp->installEventFilter(new CDownloadResponse(qApp));
  }

  logMessage(serviceName().append(" started"), QtServiceBase::Error );

  if (!m_server->isListening())
  {
    logMessage(QString("Failed to bind to port %1").arg(m_server->serverPort()), QtServiceBase::Error);
    qApp->quit();
  }
}

void CHttpDaemon::stop(void)
{
  qDebug() << "stop";
  qApp->quit();
}

void CHttpDaemon::pause(void)
{
  qDebug() << "pause";
}

void CHttpDaemon::resume(void)
{
  qDebug() << "resume";
}

void CHttpDaemon::processCommand(int code)
{
  qDebug() << "processCommand(" << code << ")";
  Q_UNUSED(code)
}
