#include "response_relay.h"

#include <util/queries.h>

#include "article.h"
#include "file.h"
#include "download.h"

bool CResponseRelay::requestFilter(CTcpRequestEvent* request)
{
  QStringList parts = QString(request->header().path()).split('/', QString::SkipEmptyParts);
#ifdef DEBUG_LOOKUP
  qDebug() << parts;
#endif
  QString path = parts.takeFirst();

  if(path == "stylesheets")
  {
    request->socket()->write("HTTP/1.1 200 Ok\r\n"
                             "Content-type: text/css; charset=\"utf-8\"\r\n"
                             "\r\n");
    QFile css(QString(":%1").arg(request->header().path().data()));
    css.open(QIODevice::ReadOnly);
    request->socket()->write(css.readAll());
  }
  else if(path == "files" || path == "images")
  {
    CDownloadRequestEvent* event_ = new CDownloadRequestEvent();
    event_->setSocket(request->socket());
    event_->setData(parts);
    QCoreApplication::postEvent(qApp, event_, Qt::RealTimeEventPriority);
  }
  else
  {
    ENamespaces ns = ENamespaces::eArticle;
    int pos = path.indexOf(':');
    if(pos != -1)
    {
      ns = nsmap.value(path.mid(pos + 1), ENamespaces::BADVALUE);
      path = path.left(pos);
    }

    vector_t<vector_t<QVariant>> results = QueryThread::fastQuery(query("check page exists in namespace"), static_cast<int>(ns), path);

    if(results.isEmpty())
    {
#ifdef DEBUG_LOOKUP
      qDebug() << "404!" << path;
#endif
    }
    else if(ns == ENamespaces::eArticle) // article
    {
      CArticleRequestEvent* event_ = new CArticleRequestEvent();
      event_->setSocket(request->socket());
      event_->setPath  (path);
      event_->setTitle (results[0][0].toByteArray());
      event_->setArguments(request->header().arguments());
      QCoreApplication::postEvent(qApp, event_, Qt::RealTimeEventPriority);
    }
    else if(ns == ENamespaces::eFile) // file page
    {
      CFileRequestEvent* event_ = new CFileRequestEvent();
      event_->setSocket  (request->socket());
      event_->setPath    (path);
      event_->setFileName(results[0][0].toByteArray());
      //event_->setArguments(request->header().arguments());
      QCoreApplication::postEvent(qApp, event_, Qt::RealTimeEventPriority);
    }
  }

  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::X11ExcludeTimers);

  return false;
}
