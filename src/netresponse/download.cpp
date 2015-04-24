#include "download.h"

#include <util/filecache.h>

QSize img_sz(const QString& sz)
{
  if(!sz.endsWith("px"))
    return QSize();

  int vals[2] = {0};
  auto str_pos = sz.begin();
  auto str_end = sz.end();
  --str_end;
  --str_end;

  for(; str_pos != str_end && str_pos->isDigit(); ++str_pos)
  {
    vals[0] *= 10;
    vals[0] += str_pos->digitValue();
  }

  if(str_pos != str_end && *str_pos == 'x')
  {
    for(++str_pos; str_pos != str_end && str_pos->isDigit(); ++str_pos)
    {
      vals[1] *= 10;
      vals[1] += str_pos->digitValue();
    }
  }

  if(str_pos == str_end)
    return QSize(vals[0], vals[1]);

  return QSize();
}

void CDownloadRequestEvent::setData (const QStringList& data)
{
  m_timestamp.setTime(QTime());
  m_timestamp.setDate(QDate());
  m_size = QSize();
  m_filename.clear();

  for(auto pos = data.begin(); pos != data.end(); ++pos)
  {
    bool ok = !m_size.isNull();
    if(!ok) // if size is null
      setSize(img_sz(*pos));

    if(ok || m_size.isNull()) // size already filled or size conversion failed
    {
      ok = !m_timestamp.isValid();
      if(!ok)
        setTimestamp(QDateTime::fromString(*pos, "yyyyMMddhhmmss"));

      if(ok || !m_timestamp.isValid()) // timestamp already filled or timestamp conversion failed
        setFileName(pos->toUtf8());
    }
  }

#ifdef DEBUG_LOOKUP
  if(size().isValid())
    qDebug() << "size:" << size();
  if(timestamp().isValid())
    qDebug() << "timestamp:" << timestamp();
  if(!fileName().isEmpty())
    qDebug() << "filename:" << fileName();
#endif
}

const HUString message404 ("<html>\r\n  <head>\r\n    <title>404 File Not Found</title>\r\n  </head>\r\n  <body>\r\n404 File Not Found\r\n  </body>\r\n</html>");

bool CDownloadResponse::requestFilter(CDownloadRequestEvent* request)
{
  vector_t<vector_t<QVariant>> results;

  HUString querystr = "image data from name";
  if(request->timestamp().isValid())
    querystr = "image data from timestamp and name";

  if(request->size().width() > 0)
    querystr += "sort by width";
  else if(request->size().height() > 0)
    querystr += "sort by height";

  querystr = query(querystr);

  if(request->timestamp().isValid())
    results = QueryThread::fastQuery(querystr, request->timestamp().toString("yyyyMMddhhmmss"), request->fileName());
  else
    results = QueryThread::fastQuery(querystr, request->fileName());

  if(results.isEmpty())
  {
    request->socket()->write("HTTP/1.1 400 Bad Request\r\n"
                             "Content-Type: text/html; charset=\"utf-8\"\r\n"
                             "\r\n");
    request->socket()->write(message404);
  }
  else
  {
    QTextStream os(reinterpret_cast<QIODevice*>(request->socket()));
    os.setAutoDetectUnicode(true);

    request->socket()->write("HTTP/1.1 200 OK\r\n"
                             "Content-Type: image/png\r\n"
                             "Connection: close\r\n\r\n");

    request->socket()->write(results[0][2].toByteArray());
  }

  return false;
}
