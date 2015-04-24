#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <QtCore>
#include <soloutils>

class CHttpHeader : public QHash<hashed_t, QByteArray>
{
public:
  CHttpHeader(void){}

  bool read(QIODevice* dev);

  //inline void setMethod   (const QByteArray& m) { m_method    = m; }
  //inline void setPath     (const QByteArray& p) { m_path      = p; }
  //inline void setProtocol (const QByteArray& p) { m_protocol  = p; }
  //inline void setVersion  (const QByteArray& v) { m_version   = v; }


  inline const QByteArray& method   (void) const { return m_method   ; }
  inline const QByteArray& path     (void) const { return m_path     ; }
  inline const QByteArray& protocol (void) const { return m_protocol ; }
  inline const QByteArray& version  (void) const { return m_version  ; }

  inline const list_t<KVPair>& arguments(void) const { return m_arguments; }
  inline const list_t<KVPair>& postdata (void) const { return m_postdata ; }

private:
  void setArguments(const QByteArray& argdata);
  inline bool parseLine(const QByteArray& line) { return parseLine(*this, line); }
  static bool parseLine(QHash<hashed_t, QByteArray>& data, const QByteArray& line);

private:
  list_t<KVPair> m_arguments;
  list_t<KVPair> m_postdata;
  QByteArray m_method, m_path, m_protocol, m_version;
};

#endif // HTTP_HEADER_H
