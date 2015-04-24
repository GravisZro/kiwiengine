#ifndef FILE_H
#define FILE_H

#include "response_template.h"

class CFileRequestEvent : public CRequestEventTemplate<Qt::WikiFile>
{
public:
  const HUString& fileName (void) const { return m_filename; }
  void setFileName (const HUString& _filename) { m_filename = _filename ; }
private:
  HUString m_filename;
};

class CFileResponse : public CResponseTemplate<CFileRequestEvent, Qt::WikiFile>
{
public:
  CFileResponse(QObject* parent = nullptr) : CResponseTemplate(parent) { }
  bool requestFilter(CFileRequestEvent* request);
};

#endif // FILE_H
