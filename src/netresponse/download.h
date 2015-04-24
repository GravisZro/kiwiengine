#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "response_template.h"

class CDownloadRequestEvent : public CRequestEventTemplate<Qt::WikiDownload>
{
public:
  void setData (const QStringList& data); // figure out what's what and set the fields

  inline const HUString & fileName  (void) const { return m_filename ; } // original filename
  inline const QSize    & size      (void) const { return m_size     ; } // dimensions (for image files)
  inline const QDateTime& timestamp (void) const { return m_timestamp; } // time/date uploaded

  inline void setFileName (const HUString & _filename ) { m_filename  = _filename ; }
  inline void setSize     (const QSize    & _size     ) { m_size      = _size     ; }
  inline void setTimestamp(const QDateTime& _timestamp) { m_timestamp = _timestamp; }

private:
  HUString  m_filename;
  QSize     m_size;
  QDateTime m_timestamp;
};

class CDownloadResponse : public CResponseTemplate<CDownloadRequestEvent, Qt::WikiDownload>
{
public:
  CDownloadResponse(QObject* parent = nullptr) : CResponseTemplate(parent) { }
  bool requestFilter(CDownloadRequestEvent* request);
};

#endif // DOWNLOAD_H
