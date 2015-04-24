#ifndef HTTP_REQUEST_EVENT_H
#define HTTP_REQUEST_EVENT_H

#include <QtCore>
#include <QtNetwork>
#include <QString>

#include <soloutils>

namespace Qt
{
  enum ECustomEventPriority : int32_t
  {
    RealTimeEventPriority = INT32_MAX // the absolute highest priority
  };

  enum ECustomEventType : quint32
  {
    TCPRequest = QEvent::User,
    WikiArticle,
    WikiEditArticle,
    WikiFile,
    WikiDownload,
  };
}

template<quint32 RequestType>
class CRequestEventTemplate : public QEvent
{
public:
  CRequestEventTemplate(void) : QEvent(static_cast<QEvent::Type>(RequestType)) { }

  const QString& path  (void) const { return m_path  ; }
  QTcpSocket*    socket(void)       { return m_socket; }

  void setPath  (const QString& _path  ) { m_path   = _path  ; }
  void setSocket(QTcpSocket*    _socket) { m_socket = _socket; }

  inline const list_t<KVPair>& arguments (void) const { return m_arguments; }
  inline void setArguments (const list_t<KVPair>& _arguments) { m_arguments = _arguments; }

private:
  QString m_path;
  list_t<KVPair> m_arguments;
  QTcpSocket* m_socket;
};

#endif // HTTP_REQUEST_EVENT_H
