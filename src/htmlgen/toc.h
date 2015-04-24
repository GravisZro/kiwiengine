#ifndef WIKI_TOC_H
#define WIKI_TOC_H

#include <globalvars.h>
#include <htmlgen/element.h>

const bool global_enable = wgThumbLimits[wgDefaultUserOptions.find("showtoc")->toBool()];
const int global_max_depth = wgMaxTocLevel;

class CTocGenerator : public CElement
{
public:
  CTocGenerator(void) : m_max_depth(global_max_depth), m_enabled(global_enable) { }
  template<typename T> inline CTocGenerator (const T& other) { *this = other; }
  template<typename T> inline CTocGenerator& operator =(const T& other)
    { m_sections.clear(); *reinterpret_cast<QDomElement*>(this) = other.toElement(); return *this; }

  inline void disable(void) { m_enabled = false; }
  inline bool hasParent(void) const { return !parentNode().isNull(); }
  inline void setMaxDepth(unsigned int max) { m_max_depth = max; }
  inline void addSection(const HUString& title, const unsigned int& depth)
  {
#if DEBUG_TOC_PARSER
    qDebug() << "added to ToC:" << title;
    qDebug() << "  depth:" << depth;
#endif
    m_sections.emplace_back(title, depth);
  }

  void render(void);

private:
  int m_max_depth;
  bool m_enabled;
  list_t<pair_t<HUString, int>> m_sections;
};

#endif // WIKI_TOC_H
