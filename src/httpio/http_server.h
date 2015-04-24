#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtNetwork>

class CHttpServer : public QTcpServer
{
  Q_OBJECT
public:
  explicit CHttpServer(const quint16 port, QObject* parent = 0);

  void incomingConnection(qintptr socket);

  bool isPaused(void) const;
  void pause (void);
  void resume(void);

private slots:
  void readClient(void);
  void discardClient(void);

private:
  bool m_paused;
  //QMap<QByteArray, CHttpHeader> m_multipart_buffer;
};

#endif // HTTPSERVER_H
