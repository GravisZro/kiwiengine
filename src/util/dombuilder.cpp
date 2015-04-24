#include "dombuilder.h"
#include <soloutils>

void build_attribute_list(QDomElement& current, const QVariant& dom)
{
  if(dom.canConvert(QMetaType::QStringList))
  {
    QStringList strlist = dom.toStringList();
    Q_ASSERT(strlist.size() == 2);
    current.setAttribute(strlist.front(), strlist.back());
  }
  else if(dom.canConvert(QMetaType::QVariantList))
  {
    QList<QVariant> list = dom.toList();
    for(auto pos = list.begin(); pos != list.end(); ++pos)
      build_attribute_list(current, *pos);
  }
  else { Q_ASSERT(false); }
}

void DomBuilderTHROW(QDomElement& current, const QVariant& dom)
{
  Q_ASSERT(dom.canConvert(QMetaType::QVariantList));
  QList<QVariant> list = dom.toList();

  if(list.first().canConvert(QMetaType::QString)) // html tag
  {
    auto pos = list.begin();

    switch(HUString(*pos))
    {
      case "COMMAND:STOP"_hash:
        throw(current);
        break;
      default: // tag
      {{
        current.setTagName(pos->toString());
        ++pos;
        if(pos->canConvert(QMetaType::QString)) // tag text
          current.appendChild(current.ownerDocument().createTextNode(pos->toString()));
        else if(!pos->toList().front().canConvert(QMetaType::QString)) // not a new tag
        {
          build_attribute_list(current, *pos);
          ++pos;
        }

        for(; pos != list.end(); ++pos) // iterate children
        {
          QDomElement child;
          child = current.appendChild(child).toElement();
          DomBuilder(child, *pos);
        }
      }}
    }
  }
  else // multiple tags
  {
    for(auto pos = list.begin(); pos != list.end(); ++pos)
      DomBuilder(current, *pos);
  }
}

QDomElement& DomBuilder(QDomElement& current, const QVariant& dom)
{
  try
  {
    DomBuilderTHROW(current, dom);
  }
  catch(QDomElement& stop_node)
  {
    return stop_node;
  }
  return current;
}


void DomElement::print(void) const
{
  if(!name.isEmpty())
  {
    if(attrs.isEmpty() && children.isEmpty())
    {
      qDebug() << "text:" << name;
    }
    else
    {
      QString out("<");
      out.append(name);

      for(auto pos = attrs.begin(); pos != attrs.end(); ++pos)
      {
        out.append(QString(" %1=\"%2\"").arg(pos->first).arg(pos->second));
      }
      out.append(">\n");
      qDebug(qPrintable(out));
    }
  }

  for(auto pos = children.begin(); pos != children.end(); ++pos)
    pos->print();

  if(!name.isEmpty())
    qDebug(qPrintable(QString("</%1>").arg(name)));
}
