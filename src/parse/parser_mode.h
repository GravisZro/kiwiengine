#ifndef PARSER_MODE_H
#define PARSER_MODE_H

#include <soloutils>

class CParserMode
{
public:
  enum EMode : unsigned int
  {
    Normal            = ""_hash,

    HtmlTable         = "<table>"_hash,
    HtmlOrderedList   = "<ol>"_hash,
    HtmlUnorderedList = "<ul>"_hash,

    BBTable           = "[table]"_hash,
    BBList            = "[list]"_hash,

    WikiIndent        = "wikiindent"_hash,
    WikiTableLine     = "wikitableline"_hash,
    WikiTable         = "wikitable"_hash,
    WikiList          = "wikilist"_hash,

    CSS               = "<css>"_hash,

    HtmlCode          = "<code>"_hash,
    BBCode            = "[code]"_hash,

    NoWiki            = "<nowiki>"_hash,
    NoBB              = "[nobb]"_hash,
    NoParse           = "<noparse>"_hash,

    IncludeOnly       = "<includeonly>"_hash,
    OnlyInclude       = "<onlyinclude>"_hash,
    NoInclude         = "<noinclude>"_hash,
  };

  inline bool operator == (const EMode& mode) const { return m_mode.contains(mode); }
  inline bool operator != (const EMode& mode) const { return !operator ==(mode); }

  void operator =(const hashed_t& mode_name);
  //inline void operator =(const unsigned int& mode_name_hash) { operator =(hashed_t(mode_name_hash)); }

  inline const bool& wikicode (void) const { return current().wikicode ; }
  inline const bool& bbcode   (void) const { return current().bbcode   ; }

  inline int list_depth(void) const { return m_mode.count(WikiList); }

  inline void reset(void) { m_mode.clear(); }

  operator HUString(void) const;

private:
  struct parse_mode_tag_t
  {
    bool wikicode;  // is wikicode enabled?
    bool bbcode;    // is bbcode enabled?
    bool stackable; // possible to stack other modes on top of this mode?
    hashed_t opener;
    hashed_t closer;
  };

  inline const parse_mode_tag_t& current(void) const
    { return *m_modes.find(m_mode); }
  //{ return m_modes.find(m_mode)->second; }

  struct CHashStack : stack_t<unsigned int>
  {
    inline operator unsigned int(void) const { return empty() ? Normal : top(); }
    inline void operator =(const unsigned int& mode) { push(mode); }
  };

  CHashStack m_mode;

  const map_t<unsigned int, parse_mode_tag_t> m_modes =
    {
      { Normal            , { true , true , true , ""              , ""               } },

    // parser enablers
      { HtmlTable         , { true , true , true , "<table>"       , "</table>"       } },
      { HtmlOrderedList   , { true , true , true , "<ol>"          , "</ol>"          } },
      { HtmlUnorderedList , { true , true , true , "<ul>"          , "</ul>"          } },

      { BBTable           , { true , true , true , "[table]"       , "[/table]"       } },
      { BBList            , { true , true , true , "[list]"        , "[/list]"        } },

      { WikiIndent        , { true , true , true , "wikiindent"    , "/wikiindent"    } },
      { WikiTableLine     , { true , true , true , "wikitableline" , "/wikitableline" } },
      { WikiTable         , { true , true , true , "wikitable"     , "/wikitable"     } },
      { WikiList          , { true , true , true , "wikilist"      , "/wikilist"      } },

    // parser disablers
      { CSS               , { false, false, false, "<css>"         , "</css>"         } },

      { HtmlCode          , { false, false, false, "<code>"        , "</code>"        } },
      { BBCode            , { false, false, false, "[code]"        , "[/code]"        } },

      { NoWiki            , { false, true , true , "<nowiki>"      , "</nowiki>"      } },
      { NoBB              , { true , false, true , "[noparse]"     , "[/noparse]"     } },
      { NoParse           , { false, false, false, "<noparse>"     , "</noparse>"     } },

    // conditional parsing
      { IncludeOnly       , { true , true , true , "<includeonly>" , "</includeonly>" } },
      { NoInclude         , { true , true , true , "<noinclude>"   , "</noinclude>"   } },
    };
};

#endif // PARSER_MODE_H
