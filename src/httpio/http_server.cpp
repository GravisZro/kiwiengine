#include "http_server.h"

#include <QtService>

#include <netresponse/response_relay.h>
#include <util/filecache.h>

#include <soloutils>
#include "http_header.h"

#ifndef QUOTE
#define QUOTE2(x) #x
#define QUOTE(x) QUOTE2(x)
#endif

#define WIKI_HOSTNAME2 QUOTE(WIKI_HOSTNAME)


#if 0
// architecture dependant code!
static_assert(sizeof(unsigned long int) == sizeof(uint64_t), "sorry, unsupported architecture");
constexpr uint64_t combine(const char* s, std::size_t sz)
  { return (0xFFFFFFFFFFFFFFFF >> ((8 - sz) * 8)) & (*reinterpret_cast<const uint64_t*>(s)); }
// end architecture dependant code
#endif
#if 0
constexpr uint64_t combine(const char* s, std::size_t sz, std::size_t offset = 0)
 { return (offset < sz && s[offset] != ' ') ? s[offset] + (combine(s, sz, offset + 1) << 8) : 0; }

constexpr std::size_t const_strlen(const char* s, std::size_t sz = 0)
  { return (sz == 8 || !s[sz] || s[sz] == ' ') ? sz : const_strlen(s, sz + 1); }

constexpr uint64_t combine(const char* s) { return combine(s, const_strlen(s)); }
constexpr uint64_t operator "" _u64(const char* s, std::size_t sz) { return combine(s, sz); }
#endif

CHttpServer::CHttpServer(const quint16 port, QObject* parent)
  : QTcpServer(parent),
    m_paused(false)
{
  listen(QHostAddress::Any, port);
}

void CHttpServer::incomingConnection(qintptr socket)
{
  if (isPaused())
    return;

  // When a new client connects, the server constructs a QTcpSocket and all
  // communication with the client is done over this QTcpSocket. QTcpSocket
  // works asynchronously, this means that all the communication is done
  // in the two slots readClient() and discardClient().
  QTcpSocket* s = new QTcpSocket(this);
  connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
  connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
  s->setSocketDescriptor(socket);

  QtServiceBase::instance()->logMessage("New Connection");
}

bool CHttpServer::isPaused(void) const
{
  return m_paused;
}

void CHttpServer::pause(void)
{
  m_paused = true;
}

void CHttpServer::resume(void)
{
  m_paused = false;
}

void CHttpServer::readClient(void)
{
  QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
  CTcpRequestEvent* event_ = new CTcpRequestEvent();
  CHttpHeader header;

  try
  {
    if(isPaused())
      throw("output disabled");

    if(socket->bytesAvailable() <= 0 &&
       socket->bytesAvailable() >= (1 << 20)) // hard limit of 1MB input
      throw("Illegal request size.");

    if(!socket->canReadLine())
      throw("Invalid request");



    if(!header.read(socket))
      return;

    switch(hashed_t(header.method()))
    {
// HTTP 1.0
      case "GET"_hash:

        if(header.path() == "/favicon.ico")
        {
          static QByteArray favicon;
          if(favicon.isEmpty())
            favicon = CFileCache::get(1);

          socket->write("HTTP/1.1 200 OK\r\n"
                        "Content-Type: image/png\r\n"
                        "Connection: close\r\n\r\n");
          socket->write(favicon); // favicon
          throw("favicon");
        }

        if(header.path() == "/")
        {
          socket->write("HTTP/1.1 301 Moved Permanently\r\n"
                        "Location: /Main_Page\r\n");

          throw("forward to Main_Page");
        }

        if(header.path().contains("%20"))
        {
           QByteArray path = header.path();
           path = path.replace("%20", "_");
           socket->write("HTTP/1.1 301 Moved Permanently\r\nLocation: ");
           socket->write(path);
           socket->write("\r\n");

           throw("forward to proper page name");
        }


        break;
      case "HEAD"_hash:
        break;
      case "POST"_hash:
        break;

// HTTP 1.1
      case "OPTIONS"_hash:
        break;
      case "PUT"_hash:
        break;
      case "DELETE"_hash:
        break;
      case "TRACE"_hash:
        break;
      case "CONNECT"_hash:
        break;
      case "PATCH"_hash:
        break;

  // HTTP 2.x (?)
      case "MOVE"_hash:
        break;
      case "PURGE"_hash:
        break;
      case "REFRESH"_hash:
        break;

  // WebDAV
  // HTTP 1.1
      case "PROPFIND"_hash:
        break;
      case "PROPPATCH"_hash:
        break;
      case "MKCOL"_hash:
        break;
      case "COPY"_hash:
        break;
      case "LOCK"_hash:
        break;
      case "UNLOCK"_hash:
        break;

  // HTTP 2.x
      case "VERSION-CONTROL"_hash:
        break;
      case "REPORT"_hash:
        break;
      case "CHECKIN"_hash:
        break;
      case "CHECKOUT"_hash:
        break;
      case "UNCHECKOUT"_hash:
        break;
      case "UPDATE"_hash:
        break;
      case "MKWORKSPACE"_hash:
        break;
      case "LABEL"_hash:
        break;
      case "MERGE"_hash:
        break;
      case "BASELINE-CONTROL"_hash:
        break;
      case "MKACTIVITY"_hash:
        break;
      case "BIND"_hash:
        break;
      case "SEARCH"_hash:
        break;

      default:
        qDebug() << "unknown header type";
        break;
    }

    event_->setHeader(header);
    event_->setSocket(socket);

    QCoreApplication::postEvent(qApp, event_, Qt::RealTimeEventPriority); // set network request event
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::X11ExcludeTimers); // process the network request event

    QtServiceBase::instance()->logMessage("Wrote to client");
  }
  catch(const char* error_message)
  {
    QtServiceBase::instance()->logMessage(error_message);
  }

  if(socket->isOpen())
    socket->close();
  if(socket->state() == QTcpSocket::UnconnectedState)
  {
    delete socket;
    QtServiceBase::instance()->logMessage("Connection closed");
  }
}

void CHttpServer::discardClient(void)
{
  QTcpSocket* socket = (QTcpSocket*)sender();
  socket->deleteLater();

  QtServiceBase::instance()->logMessage("Connection closed");
}
