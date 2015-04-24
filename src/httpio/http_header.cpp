#include "http_header.h"

bool CHttpHeader::read(QIODevice* dev)
{
  QByteArray multipart_marker, end_marker;
  QByteArray data = dev->readAll();
  QBuffer buf(&data);
  buf.open(QIODevice::ReadOnly);

  //qDebug() << "<DATA>: " << data;
  //qDebug() << "</DATA>----------------------------------------------------------------";

  QList<QByteArray> sections = buf.readLine().trimmed().split(' ');
  m_method = sections.takeFirst();

  if(sections.isEmpty() || sections.size() > 2)
    return false;

  if(sections.size() == 2)
  {

    int idx = sections.first().indexOf('?');
    if(idx < 0)
      m_path = sections.takeFirst();
    else
    {
      m_path = sections.first().left(idx);
      setArguments(sections.takeFirst().mid(idx + 1));
    }
  }

  sections = sections.first().split('/');
  if(sections.size() != 2)
    return false;

  m_protocol = sections.first().toLower();
  m_version = sections.last();

  while(parseLine(buf.readLine().trimmed()));

  for(auto pos = find("Content-Type"); pos != end() && pos.key() == "Content-Type"_hash; ++pos)
    if(pos.value().startsWith("multipart"))
      multipart_marker = pos.value().mid(pos.value().indexOf("boundary=") + sizeof("boundary=") - 1);


  if(!multipart_marker.isEmpty())
  {
    multipart_marker.prepend("--");
    end_marker = multipart_marker + "--";

    int next = 0;
    while((next = data.indexOf(multipart_marker, buf.pos())) > 0)
    {
      buf.seek(next);
      QByteArray line = buf.readLine().trimmed();
      if(line == end_marker)
      {
        break;
      }
      else if(line == multipart_marker)
      {
        QHash<hashed_t, QByteArray> section_data;
        while(parseLine(section_data, buf.readLine().trimmed())); // parse section details data

        for(auto pos = section_data.find("Content-Disposition"); pos != section_data.end(); ++pos)
        {
          if(pos.value().startsWith("form-data;"))
          {
            int nnext = data.indexOf(multipart_marker, buf.pos());
            int start = pos.value().indexOf("name=") + sizeof("name=");
            m_postdata.emplace_back( // emplace KVPair
                  pos.value().mid(start, pos.value().indexOf('"', start + 1) - start), // key
                  buf.read(nnext - buf.pos())); // value
          }
          else
          {
            qDebug() << "unknown post data field: " << pos.value();
          }
        }
      }
      else
      {
        qDebug() << "unknown data" << line;
      }
    }

    for(auto& kv : m_postdata)
      qDebug() << "key: " << kv.key() << "\nvalue: " << kv.value() << "\n";
  }

  return true;
}

void CHttpHeader::setArguments(const QByteArray& argdata)
{
  for(QByteArray& kv : argdata.split('&'))
  {
    int idx = kv.indexOf('=');
    if(idx < 0)
      m_arguments.emplace_back(kv, kv);
    else
      m_arguments.emplace_back(kv.left(idx), kv.mid(idx + 1));
  }
}

bool CHttpHeader::parseLine(QHash<hashed_t, QByteArray>& data, const QByteArray& line)
{
    int i = line.indexOf(':');
    if (i == -1)
        return false;

    data.insertMulti(line.left(i).trimmed(), line.mid(i + 1).trimmed());
    return true;
}
