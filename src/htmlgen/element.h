#ifndef WIKI_ELEMENT_H
#define WIKI_ELEMENT_H

#include <globalvars.h>
#include <soloutils>
#include <QDomElement>

class CElement : public QDomElement
{
public:
  inline CElement(void) { }//m_buffer.reserve(102400); }
  inline CElement(const CElement& other)
    : QDomElement(other)
  {
    m_dom_stack = other.m_dom_stack;
    m_buffer = other.m_buffer;
  }

  inline CElement (const QDomElement& other) : QDomElement(other) { }
  inline CElement (const QDomNode& other) : QDomElement(other.toElement()) { }

  inline CElement& operator =(const QDomNode& other)
    { return operator =(other.toElement()); } // call operator below :)
  inline CElement& operator =(const QDomElement& other)
    { *reinterpret_cast<QDomElement*>(this) = other; return *this; }

  // QDom stuff
  inline void setParentName(QString name) { return parentNode().toElement().setTagName(name); }
  inline QString parentName(void) const { return parentNode().nodeName(); }

  HUString currentNodeName(void) const { return nodeName().toUtf8(); }

  inline void setAttribute(HUString name, HUString att) { QDomElement::setAttribute(name.data(), att.data()); }
  void setAttributes(list_t<KVPair> attributes);


  void appendComment(HUString comment);

  QDomElement appendElement(HUString tag_name);
  QDomElement appendElement(HUString tag_name, KVPair attribute);
  QDomElement appendElement(HUString tag_name, list_t<KVPair> attributes);

  CElement& becomeChild(HUString tag_name);
  CElement& becomeChild(HUString tag_name, KVPair attribute);
  CElement& becomeChild(HUString tag_name, list_t<KVPair> attributes);

  void becomeParent(void);

  // table functions
  inline void endRow       (const char* type) { endCell(type);       if(nodeName() == "tr") { becomeParent(); } }
  inline void endTable     (const char* type) { endRow (type); Q_ASSERT(nodeName() == "table" && attribute("tagtype") == type); becomeParent(); }

  inline void startRow     (const char* type) { endRow (type); Q_ASSERT(nodeName() == "table" && attribute("tagtype") == type); becomeChild("tr"     , {"tagtype", type}); }
  inline void startCell    (const char* type) { endCell(type);       if(nodeName() != "tr"                ) { startRow(type); } becomeChild("td"     , {"tagtype", type}); }
  inline void startHeader  (const char* type) { endCell(type);       if(nodeName() != "tr"                ) { startRow(type); } becomeChild("th"     , {"tagtype", type}); }
  inline void startTable   (const char* type) { flushBuffer();                                                                  becomeChild("table"  , {"tagtype", type}); }
  inline void startCaption (const char* type) { endRow (type); Q_ASSERT(nodeName() == "table" && attribute("tagtype") == type); becomeChild("caption", {"tagtype", type}); }

  // buffer stuff
  inline void appendText(const QString& hutext) { QString text = hutext; appendChild(ownerDocument().createTextNode(text)); }

  template<typename T>
  inline auto& operator <<   (const T& other) { m_buffer.append(other); return *this; }
  inline void  setBuffer     (const HUString& buffer) { m_buffer = buffer; }
  inline bool  bufferEmpty   (void) const { return m_buffer.isEmpty(); }
  inline auto& peekBuffer    (void) const { return m_buffer; }

  inline auto  dumpBuffer    (void) { auto tmp = m_buffer; m_buffer.clear(); return tmp; }
  inline void  flushBuffer   (void) { if(m_buffer.trimmed().isEmpty()) { clearBuffer(); }
                                      else { appendChild(ownerDocument().createTextNode(dumpBuffer())); } }
  inline void  trimBuffer    (void) { m_buffer = m_buffer.trimmed(); }
  inline void  clearBuffer   (void) { m_buffer.clear(); }

  inline bool stackEmpty(void) const { return m_dom_stack.isEmpty(); }
  inline const HUString& topOfStack(void) const { return m_dom_stack.top(); }
  inline bool inStack(const HUString& str) const { return m_dom_stack.contains(str); }

#ifdef DEBUG_STRICT_TAGTYPE
  inline void disableAssert(void) { m_noassert = true ; }
  inline void enableAssert (void) { m_noassert = false; }
#endif
private:
#ifdef DEBUG_STRICT_TAGTYPE
  static bool m_noassert;
#endif

  void endCell(const char* type);

  stack_t<HUString> m_dom_stack;
  HUString m_buffer;
};

#endif // WIKI_ELEMENT_H
