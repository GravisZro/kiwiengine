#include "page.h"

#include <htmlgen/element.h>

CPageGenerator::CPageGenerator(void)
  : QDomDocument("HTML")
{
  m_style = createElement("style");
  m_style.setAttribute("type", "text/css");

  m_html_title = createTextNode("HTML TITLE");
  m_body_title = createTextNode("BODY TITLE");

  CElement b = appendChild(createElement("html"));

  b.setAttributes({ {"lang", "en"}, {"dir", "ltr"}, {"class", "client-nojs"} });
  b.becomeChild("head");
    m_header = b;
    b.appendElement("meta", {"charset", "UTF-8"});
    b.becomeChild("title");
    b.appendChild(m_html_title);
    b.becomeParent();
    b.appendElement("link", { {"rel", "stylesheet"}, {"type", "text/css"}, {"href", "/stylesheets/mw.css"} });
    b.appendElement("link", { {"rel", "stylesheet"}, {"type", "text/css"}, {"href", "/stylesheets/vector.css"} });
  b.becomeParent();
  b.becomeChild("body", {"class", "mediawiki ltr sitedir-ltr ns-0 ns-subject skin-vector action-view"});// vector-animateLayout"});
    b.becomeChild("div", { {"id", "content"}, {"class", "mw-body"}, {"role", "main"} });
      b.becomeChild("h1", { {"id", "firstHeading"}, {"class", "firstHeading"}, {"lang", "en"} });
        b.becomeChild("span", {"dir", "auto"});
        b.appendChild(m_body_title);
        b.becomeParent();
      b.becomeParent();
      b.becomeChild("div", {"id", "bodyContent"});
        b.becomeChild("div", { {"id", "mw-content-text"}, {"lang", "en"}, {"dir", "ltr"}, {"class", "mw-content-ltr"} });
  m_body = b;
  m_footer = m_body.parentNode().toElement();
}
