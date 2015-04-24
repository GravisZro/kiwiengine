#ifndef WIKI_PAGE_H
#define WIKI_PAGE_H

#include <globalvars.h>

#include <netresponse/response_template.h>
#include <QDomDocument>

class CPageGenerator : public QDomDocument
{
public:
  CPageGenerator(void);

  template<quint32 RequestType>
  void render(CRequestEventTemplate<RequestType>* request)
  {
    m_html_title.setData(m_title);
    m_body_title.setData(m_title);

    if(m_style.hasChildNodes()) // if additional style stuff, add style tag
      m_header.appendChild(m_style);
    request->socket()->write("HTTP/1.1 200 Ok\r\n"
                             "Content-Type: text/html; charset=\"utf-8\"\r\n"
                             "\r\n");
    request->socket()->write(toString(-1).replace("&amp;", "&").remove("&#xd;\n").toUtf8());
  }

  // return copies
  inline QDomElement style   (void) { return m_style   ; }
  inline QDomElement body    (void) { return m_body    ; }
  inline QDomElement footer  (void) { return m_footer  ; }

  inline HUString title (void) const { return m_title; }
  inline void setTitle (const HUString& _title) { m_title = _title; }

private:
  HUString     m_title;
  QDomElement  m_header;
  QDomText     m_html_title;
  QDomText     m_body_title;
  QDomElement  m_style;
  QDomElement  m_body;
  QDomElement  m_footer;
};

#endif // WIKI_PAGE_H
