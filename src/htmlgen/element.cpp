#include "element.h"

#include <parse/allowed_tags.h>

#ifdef DEBUG_STRICT_TAGTYPE
bool element_t::m_noassert = true;
#endif

void CElement::appendComment(HUString comment)
{
  appendChild(ownerDocument().createComment(comment));
}

// ==== appendElement
QDomElement CElement::appendElement(HUString tag_name)
{
  QDomElement element = ownerDocument().createElement(tag_name);
  appendChild(element);
  return element;
}

void CElement::setAttributes(list_t<KVPair> attributes)
{
  for(auto pos = attributes.begin(); pos != attributes.end(); ++pos)
    QDomElement::setAttribute(pos->key().data(), pos->value().data());
}

QDomElement CElement::appendElement(HUString tag_name, KVPair attribute)
{
  QDomElement element = appendElement(tag_name);
  element.setAttribute(attribute.key().data(), attribute.value().data());
  return element;
}

QDomElement CElement::appendElement(HUString tag_name, list_t<KVPair> attributes)
{
  CElement element;
  element = appendElement(tag_name);
  element.setAttributes(attributes);
  return element;
}

// ==== becomeChild
CElement& CElement::becomeChild(HUString tag_name)
{
  m_dom_stack.push_back(tag_name);
  return *this = appendElement(tag_name);
}

CElement& CElement::becomeChild(HUString tag_name, KVPair attribute)
{
  m_dom_stack.push_back(tag_name);
  return *this = appendElement(tag_name, attribute);
}

CElement& CElement::becomeChild(HUString tag_name, list_t<KVPair> attributes)
{
  m_dom_stack.push_back(tag_name);
  return *this = appendElement(tag_name, attributes);
}

// ==== becomeParent
void CElement::becomeParent(void)
{
  if(parentNode().nodeType() != QDomNode::ElementNode)
    throw(complaint("not an element node"));

#ifdef DEBUG_STRICT_TAGTYPE
  Q_ASSERT(m_noassert || attribute("tagtype", "ERROR!") != "ERROR!");
#else
  removeAttribute("tagtype"); // clean up!
#endif

  if(nodeName() == "p" && !hasChildNodes()) // if empty <p> tag
  {
    //setTagName(parentNode().nodeName() == "pre" ? "nobr" : "br");

    if(parentNode().nodeName() == "pre")
    {
      QDomNode node = *this; // record self position
      *this = parentNode().toElement(); // become parent of self
      removeChild(node); // remove former self
    }
    else
    {
      setTagName("br");
      *this = parentNode().toElement(); // become parent of self
    }
  }
  else
    *this = parentNode().toElement(); // become parent of self

  Q_ASSERT(!m_dom_stack.isEmpty());
  m_dom_stack.pop_back();
}

// ==== endCell
void CElement::endCell(const char* type)
{
  flushBuffer();
  if(m_dom_stack.contains("table") && nodeName() != "table")
    while(!html::table_only.contains(nodeName()) || attribute("tagtype") != type)
      becomeParent();

  if(attribute("tagtype") == type &&
     (nodeName() == "td" ||
      nodeName() == "th" ||
      nodeName() == "caption"))
    becomeParent();
}
