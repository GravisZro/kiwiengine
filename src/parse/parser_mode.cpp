#include "parser_mode.h"

const QStringList mode_strings = { "", "<table>", "<ol>", "<ul>", "[table]", "[list]", "wikitable", "wikilist", "<css>", "<code>", "[code]", "<nowiki>", "[noparse]", "<noparse>" };

void CParserMode::operator =(const hashed_t& mode_name)
{
  if(current().closer == mode_name)
    m_mode.pop();
  else if(current().stackable)
  {
    if(m_modes.contains(mode_name))
      m_mode = mode_name;
  }
  else
    throw(QString("%1: in \"%2\" tag.  ignoring: \"%3\"")
          .arg(__func__)
          .arg(mode_strings.at(m_mode))
          .arg(mode_name));

}

CParserMode::operator HUString(void) const
{
  HUString out;
  for(unsigned int mode : m_mode)
  {
    if(!out.isEmpty())
      out.append(',');
    out.append(HUString(m_modes[mode].opener));
  }
  return out;
}
