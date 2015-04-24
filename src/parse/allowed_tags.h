#ifndef ALLOWED_TAGS_H
#define ALLOWED_TAGS_H

#include <globalvars.h>

#include <parse/container_stream.h>
#include <htmlgen/encode.h>
#include <htmlgen/element.h>

namespace html
{
  const QStringList blocks = { "div" }; // block to put in paragraphs

  const QStringList simple_pairs = { "b", "strong",        // bold
                                     "i", "em",            // italic
                                     "u", "ins",           // underline
                                     "s", "strike", "del", // strikethrough
                                     "tt", "big", "small", "sub", "sup", "p",
                                     "pre"
                                   };

  const QStringList pairs = { "b", "i", "u", "s",
                               "strong", "em", "ins", "strike", "del",
                               "tt", "big", "small", "sub", "sup",
                               "font", "h1", "h2", "h3", "h4", "h5", "h6",
                               "cite", "code", "pre", "center", "p",  "abbr",
                               "var", "div", "span", "table",
                               "blockquote", "ol", "ul", "dl",
                               "kbd", "samp", "data", "time", "mark", "bdi", "dfn",
                               "ruby", "rt", "rb", "rp" };

  const QStringList independent = { "br", "hr", "li", "dt", "dd" };
  const QStringList noclose = { "br", "hr" };

  const QStringList self_nesting = { "table", "tr", "td", "th", "div", "blockquote",
                                      "ol", "ul", "li", "dl", "dt", "dd", "font", "big",
                                      "small", "sub", "sup", "span", "var", "kbd", "samp" };
  const QStringList table_only = { "caption", "td", "th", "tr" };

  const QStringList list_tags  = { "ul", "ol", "dl" };
  const QStringList list_items = { "li", "dt", "dd" };
}

namespace bbcode
{
  bool dealias(tag_t& tag);
  bool closeTag (tag_t& tag, CElement& element);

  bool lettersToInt(const QString& input, int& value);
  bool romanToInt(const QString& input, int& value);

  const QStringList simple_pairs = { "b", "strong",        // bold
                                     "i", "em",            // italic
                                     "u", "ins",           // underline
                                     "s", "strike", "del", // strikethrough
                                     "tt", "big", "small", "sub", "sup", "p",
                                     "pre" };

  const QMap<QString /*bbcode opener*/, QStringList /*html closer(s)*/> complex_tags =
    {
      {"code"     , {"pre"}},
      {"quote"    , {"blockquote"}},
      {"list"     , {"ol","ul"}},
      {"rule"     , {}},
      {"font"     , {"font"}},
      {"size"     , {"big","small","font","h1","h2","h3","h4","h5","h6"}},
      {"abbr"     , {"abbr"}},
      {"color"    , {"span"}},
      {"highlight", {"span"}},
      {"left"     , {"span"}},
      {"right"    , {"span"}},
      {"center"   , {"span"}},
      {"table"    , {"table"}},
      {"img"      , {}},
      {"url"      , {"a"}},
      {"o"        , {"span"}},
      {"break"    , {}},
      {"*"        , {}},
    };

  const QStringList table_only = { "td", "th", "tr" };

  const QStringList listags  = { "list" };
  const QStringList list_items = { "*" };
}

namespace wikicode
{
  const QStringList formatting_pairs = { "css", "nowiki", "includeonly", "onlyinclude", "noinclude" };
  const list_t<ushort> list_items = { ECharacter::asterisk, ECharacter::hash, ECharacter::colon, ECharacter::semicolon };
}

#endif // ALLOWED_TAGS_H
