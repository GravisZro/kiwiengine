#include "toc.h"

#include <htmlgen/encode.h>

void CTocGenerator::render(void)
{
  if(m_enabled && !m_sections.isEmpty())
  {
    setAttributes({ {"id", "toc"}, {"class", "toc"} });

    CElement element;
    element = appendElement("div", {"id", "toctitle"});

      element.becomeChild("h2");
        element.appendText("Contents");
      element.becomeParent();

      element.becomeChild("span", {"class", "toctoggle"});
        element.appendText(" [");
          element.becomeChild("a", {{"id", "togglelink"}, {"href", "#"}, {"class", "internal"}});
            element.appendText("hide");
          element.becomeParent();
        element.appendText("]");
      element.becomeParent();

    // build ToC
    vector_t<int> subcount;
    stack_t<HUString> numbering;
    auto begin = m_sections.begin();
    for(auto pos = begin; pos != m_sections.end(); ++pos)
      if(pos->value() < begin->value())
        begin->value() = pos->value();
    int depth = begin->value() - 1;
    int max_depth = depth + m_max_depth;
    for(auto pos = begin; pos != m_sections.end(); ++pos)
    {
      while(depth != pos->value())
      {
        if(depth < pos->value())
        {
          ++depth;
          numbering.push("");
          for(auto subpos = subcount.begin(); subpos != subcount.end(); ++subpos)
            if(*subpos)
              numbering.top().append(QString("%1.").arg(*subpos));

          subcount.resize(pos->value());

          if(numbering.size() <= max_depth)
            element.becomeChild("ul", {"style", "display:block;"});
        }
        else if(depth > pos->value())
        {
          --depth;
          numbering.pop();
          subcount.resize(subcount.size() - 1);
          if(numbering.size() < max_depth) // beware of OBO error!
            element.becomeParent();
        }
      }

      if(depth <= max_depth)
      {
        subcount.back()++;
#ifdef DEBUG_TOC_RENDER
        qDebug(qPrintable(QString("adding   (depth: %1) title: %2").arg(pos->value()).arg(pos->key())));
#endif
        element.becomeChild("li", {"class", QString("toclevel-%1 tocsection-%2").arg(pos->value()).arg(subcount.back()).toUtf8()});
          element.becomeChild("a", {"href", "#" + encoder(pos->key())});
            element.becomeChild("span", {"class", "tocnumber"});
              element.appendText(numbering.top());
              element.appendText(QString::number(subcount.back()));
            element.becomeParent();
            element.appendText(" ");
            element.becomeChild("span", {"class", "toctext"});
              element.appendText(pos->key());
            element.becomeParent();
          element.becomeParent();
        element.becomeParent();
      }
#ifdef DEBUG_TOC_RENDER
      else
        qDebug(qPrintable(QString("ignoring (depth: %1) title: %2").arg(pos->value()).arg(pos->key())));
#endif
    }
  }
}
