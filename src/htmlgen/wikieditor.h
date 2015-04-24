#ifndef WIKIEDITOR_H
#define WIKIEDITOR_H

#include <globalvars.h>
#include <htmlgen/element.h>
#include <parse/char_stream.h>

class CWikiEditor : public CElement
{
public:
  CWikiEditor(void) { clear(); }
  template<typename T> inline CWikiEditor (const T& other) { *this = other; }
  template<typename T> inline CWikiEditor& operator =(const T& other)
    { clear(); *reinterpret_cast<QDomElement*>(this) = other.toElement(); return *this; }
  inline bool hasParent(void) const { return !parentNode().isNull(); }

  void render(void);
  void extractSection(const HUString& header, CCharStream& stream);

  inline bool enabled(void) const { return m_enabled; }
  inline const HUString& title  (void) const { return m_title  ; }
  inline const HUString& section(void) const { return m_section; }

  inline void setTitle  (const HUString& t) { m_title   = decoder(t); }
  inline void setSection(const HUString& s) { m_section = decoder(s); }
  inline void setPreview(bool p) { m_preview = p; }
  inline void setEnabled(bool e) { m_enabled = e; }

private:
  void clear(void);
  bool m_preview;
  bool m_enabled;
  HUString m_title;
  HUString m_section;
  HUString m_sectiontext;
};

#endif // WIKIEDITOR_H
