#ifndef WIKIDAEMON_H
#define WIKIDAEMON_H

#include <QtService>


class CHttpServer;

class CHttpDaemon : public QtService<QCoreApplication>
{
public:
  CHttpDaemon(const char* name, const char* description, const quint16 port, int& argc, char **argv);
 ~CHttpDaemon(void);

protected:
  void start  (void);
  void stop   (void);
  void pause  (void);
  void resume (void);
  void processCommand(int code);

private:
  CHttpServer* m_server;
  const quint16 m_port;
};

#endif // WIKIDAEMON_H
