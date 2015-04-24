#ifndef BASEGENERATOR_H
#define BASEGENERATOR_H

#include "response_template.h"
#include <httpio/http_header.h>

class CTcpRequestEvent : public CRequestEventTemplate<Qt::TCPRequest>
{
public:
  inline void setHeader(const CHttpHeader& h) { m_header = h; }
  inline const CHttpHeader& header (void) const { return m_header; }
private:
  CHttpHeader m_header;
};

class CResponseRelay : public CResponseTemplate<CTcpRequestEvent, Qt::TCPRequest>
{
public:
  CResponseRelay(QObject* parent = nullptr) : CResponseTemplate(parent) { }
  bool requestFilter(CTcpRequestEvent* request);
};

#endif // BASEGENERATOR_H
