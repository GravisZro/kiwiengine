#include "code.h"

#include <htmlgen/encode.h>
#ifdef NATIVE_RENDERING
#include <htmlgen/module/image.h>
#include <htmlgen/module/template.h>
#endif
#include <htmlgen/lua_sandbox.h>

#include <parse/container_stream.h>
#include <parse/allowed_tags.h>


QMap<hashed_t, hashed_t> renderers =
{
  { "File"           , "Native" },
  { "Image"          , "Native" },
  { "Audio"          , "Native" },
  { "Video"          , "Native" },
  { "Media"          , "Native" },

  { "Special"        , "Native" },
  { "User"           , "Native" },
  { "Project"        , "Native" },
  { "MediaWiki"      , "Native" },
  { "Template"       , "Native" },
  { "Help"           , "Native" },
  { "Category"       , "Native" },

  { "Talk"           , "Native" },
  { "User talk"      , "Native" },
  { "Project talk"   , "Native" },
  { "File talk"      , "Native" },
  { "MediaWiki talk" , "Native" },
  { "Template talk"  , "Native" },
  { "Help talk"      , "Native" },
  { "Category talk"  , "Native" },
};

inline bool in_table_cell_style(CCharStream& stream)
{
  int old_pos = stream.pos();
  try
  {
    stack_t<char> levels;
    while(stream++ != ECharacter::endline &&
          !stream.atEnd())
    {

      switch (stream)
      {
        case ECharacter::open_bracket:
        case ECharacter::open_curlybracket:
        case ECharacter::open_anglebracket:
#ifdef DEBUG_STYLE_DETECT
          qDebug() << "found closing bracket";
#endif
          levels.push(stream);
          break;

        case ECharacter::close_bracket:
        case ECharacter::close_curlybracket:
        case ECharacter::close_anglebracket:
#ifdef DEBUG_STYLE_DETECT
          qDebug() << "found opening bracket";
#endif
          if(!levels.isEmpty() && levels.top() == stream)
            levels.pop();
          break;

        case ECharacter::pipe:
#ifdef DEBUG_STYLE_DETECT
          qDebug() << "found pipe";
#endif
          if(levels.isEmpty())
            throw(stream.next() != ECharacter::pipe); // found "|" or "||"
          break;

        case ECharacter::exclaimation:
#ifdef DEBUG_STYLE_DETECT
          qDebug() << "found exclaimation";
#endif
          if(levels.isEmpty())
            if(stream.next() == ECharacter::exclaimation)
              throw(false); // found "!!"
          break;

        default:
#ifdef DEBUG_STYLE_DETECT
          qDebug(qPrintable(QString("ignoring: %1 '%2'")
                            .arg(stream.operator ushort(), 0, 16)
                            .arg(stream.operator QChar())));
#endif
          break;
      }
    }
#ifdef DEBUG_STYLE_DETECT
    qDebug() << "end of line";
#endif
    throw(false); // end of line
  }
  catch(bool result)
  {
#ifdef DEBUG_STYLE_DETECT
    if(result)
      qDebug() << "it IS a style field!\n\n\n";
    else
      qDebug() << "it is NOT a style field!\n\n\n";
#endif
    stream.seek(old_pos); // return to original position
    return result;
  }
}

inline void parse_table_style(CCharStream& stream, CElement& element)
{
  Q_ASSERT(element.bufferEmpty());
  HUString name;
  int end = stream.indexInLine(ECharacter::pipe);
  bool in_quote = false;
#ifdef DEBUG_STYLE_PARSER
  qDebug(qPrintable(QString("\nstyle chars: \"%1\"").arg(QStringRef(&stream.data(), stream.pos(), end - stream.pos()).toString())));
#endif
  while(stream.pos() < end)
  {
    while(stream.pos() < end && stream.isSpace()) { ++stream; }
    while(stream.pos() < end && stream.isLetterOrNumber())
      { element << stream; ++stream; }
    name = element.dumpBuffer();
    while(stream.pos() < end && (stream.isSpace() || stream == ECharacter::equals_sign)) { ++stream; }
    while(stream.pos() < end && (in_quote || !stream.isSpace()))
    {
      if(stream == ECharacter::quotation) { in_quote ^= true; }
      else { element << stream; }
      ++stream;
    }
    if(!name.isEmpty())
    {
#ifdef DEBUG_STYLE_PARSER
      qDebug(qPrintable(QString("   attribute: \"%1\"\n   value    : \"%2\"\n").arg(name).arg(element.peekBuffer())));
#endif
      element.setAttribute(name, element.dumpBuffer());
    }
  }
}

inline void parse_if_in_table_style(CCharStream& stream, CElement& element)
{
  if(in_table_cell_style(stream))
    parse_table_style(stream, element);
  else
    element.flushBuffer(); // print non-style table cell
}


inline void simple_parse(CElement& element, CContainerStream& stream)
{
  if(element.nodeName() != "p" &&
     html::blocks.contains(element.nodeName()) &&
    !html::list_items.contains(element.nodeName()))
    element.becomeChild("p", {"tagtype", "wiki"});
  element << stream;
}

HUString CCodeGenerator::lookup(const HUString& _title)
{
  vector_t<vector_t<QVariant>> results = QueryThread::fastQuery(query("latest page"), encoder(_title));
  Q_ASSERT(results[0][1].value<ENamespaces>() == ENamespaces::eArticle); // namespace
  return results[0][2].toByteArray();
}

void CCodeGenerator::parse(CElement& element,
                   stack_t<HUString>& lineage,
                   const bool embedded)
{
  CContainerStream stream = lookup(lineage.top());
  parse(element, lineage, embedded, stream);
}

void CCodeGenerator::parse(CElement& element,
                   stack_t<HUString>& lineage,
                   const bool embedded,
                   CContainerStream& stream)
{
  list_t<KVPair> args;
  auto attrs = element.attributes();
  for(int i = 0; i < attrs.count(); ++i)
    args.emplace_back(attrs.item(i).nodeName().toUtf8(), attrs.item(i).nodeValue().toUtf8());
  parse(element, lineage, embedded, stream, args);
}

void CCodeGenerator::parse(CElement& element,
                   stack_t<HUString>& lineage,
                   const bool embedded,
                   CContainerStream& stream,
                   list_t<KVPair> args)
{
  tag_t tag;
  QList<ushort> current_list;

  for(; !stream.atEnd(); ++stream)
  {
    // NOTE: parser can be in multiple modes at the same time!

    if(embedded) // only for child pages (transcluded)
    {
      if(m_mode == CParserMode::NoInclude) // <noinclude> handler
      {
        while(stream != "</noinclude>") { ++stream; }
        stream += sizeof("</noinclude>") - 1;
        m_mode = "</noinclude>";
      }
    }
    else // only for parent pages
    {
      // <noinclude> handler
      if(m_mode == CParserMode::IncludeOnly)
      {
        while(stream != "</includeonly>") { ++stream; }
        stream += sizeof("</includeonly>") - 1;
        m_mode = "</includeonly>";
      }
    }

    // <css> handler
    if(m_mode == CParserMode::CSS)
    {
      if(stream == "</css>")
      {
        element << ECharacter::endline;
        stream += 5; // skip css closing tag
        m_mode = "</css>";
        CElement(style()).appendText(element.dumpBuffer());
      }
      else
        element << stream;
      continue;
    }
    // </css>

// FIRST CHARACTER
    if(m_mode.wikicode() && // if current parsing mode allows wikicode at line start
       stream.prev() == ECharacter::endline)
    {
      element.flushBuffer(); // wikicode doesnt span multiple lines (unless privately parsed elsewhere)

      if(stream == ECharacter::tab ||
         stream == ECharacter::space)
      {
        if(m_mode != CParserMode::WikiIndent)
        {
          m_mode = "wikiindent";
          element.becomeChild("pre", {"tagtype", "wiki"});
        }
        continue;
      }
      else if(stream != ECharacter::endline) // if new line isnt empty
      {

        if(m_mode == CParserMode::WikiTableLine)
          m_mode = "/wikitableline";

        if(m_mode == CParserMode::WikiIndent)
        {
          m_mode = "/wikiindent";
          if(element.inStack("pre"))
          {
            while(element.tagName() != "pre")
              element.becomeParent();
            element.becomeParent();
          }
        }
        else
        {
          if(((m_mode == CParserMode::HtmlTable ||
               m_mode == CParserMode::BBTable   ||
               m_mode == CParserMode::WikiTable) &&
               stream != ECharacter::pipe &&
               stream != ECharacter::exclaimation &&
               html::table_only.contains(element.nodeName())) || // if in a table OR

             ((m_mode == CParserMode::HtmlUnorderedList ||
               m_mode == CParserMode::HtmlOrderedList   ||
               m_mode == CParserMode::BBList            ||
               m_mode == CParserMode::WikiList) &&
              stream != ECharacter::asterisk &&
              stream != ECharacter::hash &&
              stream != ECharacter::colon &&
              stream != ECharacter::semicolon &&
              html::list_items.contains(element.nodeName())) )  // in a list
          {
            element.becomeChild("p", {"tagtype", "wiki"}); // start a new paragraph
          }
        }
      }

      if(wikicode::list_items.contains(stream)) // if it's a wiki list
      {
        auto pos = current_list.begin();
        while(pos != current_list.end() && *pos == stream)
        {
          ++pos;
          ++stream;
        }

        const bool ended = pos == current_list.end();

        if(!ended || // mismatch in list OR
           wikicode::list_items.contains(stream)) // more nested list elements
        {
          for(; pos != current_list.end(); pos = current_list.erase(pos))
          {
            element.becomeParent(); // close list item
            element.becomeParent(); // close list
          }

          HUString tag_name = element.currentNodeName();
          if(!ended && html::list_items.contains(tag_name))
          {
            element.becomeParent();
            element.becomeChild(tag_name);
          }

          for(; wikicode::list_items.contains(stream); ++stream)
          {
            current_list.push_back(stream);
            switch(stream)
            {
              case ECharacter::colon    : element.becomeChild("dl"); element.becomeChild("dd"); break;
              case ECharacter::semicolon: element.becomeChild("dl"); element.becomeChild("dt"); break;
              case ECharacter::hash     : element.becomeChild("ol"); element.becomeChild("li"); break;
              case ECharacter::asterisk : element.becomeChild("ul"); element.becomeChild("li"); break;
              default:
                Q_ASSERT(false);
            }
          }
          --stream; // go back one for the character that wasnt a list item
        }
        else // just another item on the list
        {
          if(html::list_items.contains(element.nodeName()))
            element.becomeParent();

          switch(--stream)
          {
            case ECharacter::colon    : element.becomeChild("dd"); break;
            case ECharacter::semicolon: element.becomeChild("dt"); break;
            case ECharacter::hash     : element.becomeChild("li"); break;
            case ECharacter::asterisk : element.becomeChild("li"); break;
            default:
              Q_ASSERT(false);
          }
        }

        element.setAttribute("tagtype", "wiki");
        continue;
      }
      else
      {
        for(auto pos  = current_list.begin();
                 pos != current_list.end();
                 pos  = current_list.erase(pos))
        {
          if(html::list_items.contains(element.nodeName()))
            element.becomeParent();

          if(html::list_tags.contains(element.nodeName()))
            element.becomeParent();
        }
      }

      switch(stream) // process the special first character
      {
        case ECharacter::endline:
          if(element.inStack("pre"))
            element.appendText("\r\n");

          if(element.nodeName() == "p")
            element.becomeParent();
          element.becomeChild("p", {"tagtype", "wiki"});
          break;

        case ECharacter::equals_sign: // (possibly) header
          if(find_match<ECharacter::equals_sign>(stream)) // if it /is/ a header
          {
            if(!m_toc.hasParent()) // add Table of Contents
              element.appendChild(m_toc);

            while(element.attribute("id") != "mw-content-text" && // do not exit document text containter
                  element.tagName() != "pre" &&
                  !html::table_only.contains(element.tagName())) // do not exit tables
              { element.becomeParent(); }

            // this is a hack
            if(element.lastChild().nodeName() == "br") // if there is break tag before this
              element.removeChild(element.lastChild()); // delete break tag (assumed to be converted from <p></p>)
            // end of hack

            while(stream++.pos() < m_start);
            while(stream++.pos() != m_end)
              { element << stream; }

            HUString section = element.peekBuffer().trimmed();

            element.becomeChild('h' + HUString(m_depth));
              element.setAttribute("tagtype", "wiki");
              element.trimBuffer();
              element.becomeChild("span", {{"id", encoder(section)}, {"class", "mw-headline"}});
                element.setAttribute("tagtype", "wiki");
                m_toc.addSection(section, m_depth);
                element.flushBuffer();
              element.becomeParent();

              element.becomeChild("span", {"class", "mw-editsection"});
                element.becomeChild("span", {"class", "mw-editsection-bracket" });
                  element.appendText("[");
                element.becomeParent();
                  element.becomeChild("a", {"href", encoder(lineage.top()) + "?action=edit&section=" + encoder(section)});
                  element.appendText("edit");
                element.becomeParent();
                element.becomeChild("span", {"class", "mw-editsection-bracket" });
                  element.appendText("]");
                element.becomeParent();
              element.becomeParent();

            element.becomeParent();

            while(--m_depth && stream++);

            if(//!embedded && // never edit embedded pages
               m_edit.enabled() &&
               m_edit.parentNode().isNull() &&
               m_edit.section() == section)
            {
              m_edit.extractSection(section, stream);
              element.appendChild(m_edit);
            }
            continue;
          }
          break;

        case ECharacter::open_curlybracket: // open table!
          if(stream.next() == ECharacter::pipe)
          {
            stream+=2;
            m_mode = "wikitable";
            element.startTable("wiki");
            parse_table_style(stream, element);
            continue;
          }
          break;

        case ECharacter::dash: // (possibly) horizontal rule
          if(stream == "----")
          {
            element.appendElement("hr");
            while(stream++.next() == ECharacter::dash);
            continue;
          }
          break;

        case ECharacter::exclaimation:
        case ECharacter::pipe:
          if(m_mode == CParserMode::WikiTable)
          {
            m_mode = "wikitableline";
            ++stream;
            switch(stream)
            {
              case ECharacter::close_curlybracket: // end of table!
                element.endTable("wiki");
                m_mode = "/wikitable";
                continue;

              case ECharacter::plus: // table caption
                element.startCaption("wiki"); // cannot be styled
                continue;

              case ECharacter::dash: // new table row
                element.startRow("wiki"); // force a new row to start
                ++stream; // skip dash character
                parse_table_style(stream, element);
                continue;

              case ECharacter::pipe:
                element.startCell("wiki");
                parse_if_in_table_style(stream, element);
                continue;

              case ECharacter::exclaimation:
                if(stream.prev() == ECharacter::exclaimation)
                {
                  element.startHeader("wiki"); // cannot be styled
                  continue;
                }

              default: // non special character
                if(stream.prev() == ECharacter::pipe)
                  element.startCell("wiki"); // normal table cell
                else
                  element.startHeader("wiki"); // header table cell
                parse_if_in_table_style(stream, element);
                continue;
            }
          }
        default:
          if(m_mode == CParserMode::WikiTable &&
             element.nodeName() == "table") // incorrectly formatted wiki table!
          {
            element.endTable("wiki");
            m_mode = "/wikitable";
          }
          break;
      }
    }

 // PARSE NORMAL CHARACTERS
    m_start = stream.pos();

    if(!stream.atEnd())
    {
      switch(stream)
      {
        case ECharacter::endline:
          if(m_mode.wikicode() &&
             !element.inStack("pre"))
             element << ECharacter::space;
          else
            simple_parse(element, stream);
          break;

        case ECharacter::apostrophe: // (possibly) italic/bold/oblique markers
          if(stream.next() == ECharacter::apostrophe && // need at least two deep
             find_match<ECharacter::apostrophe>(stream))
          {
            switch(m_depth) // convert these into html tags then return to start of loop
            {
              case 2: replace_match(stream, "<i>"   , "</i>"    ); continue; // italic
              case 3: replace_match(stream, "<b>"   , "</b>"    ); continue; // bold
              case 5: replace_match(stream, "<i><b>", "</b></i>"); continue; // oblique
            }
          }
          simple_parse(element, stream);
          break;

        case ECharacter::open_anglebracket:
        case ECharacter::open_bracket:
        case ECharacter::open_curlybracket:
          element.flushBuffer(); // tags use the buffer

          try
          {
            bool enable_bbcode = m_mode.bbcode();
            bool enable_wikicode = m_mode.wikicode();
            if(!stream.readTag(tag, m_mode))
              throw(InvalidTag);

            if(tag.id == SingleBracketBBCode)
              bbcode::dealias(tag); // ok, so i padded bbcode a little, this will return it to "proper" bbcode

// CLOSE TAG
            if(tag.tag_name.at(0) == ECharacter::slash &&
               tag.tag_namespace.isEmpty()) // if closing tag
            {
              switch(tag.id)
              {
                case SingleBracketBBCode:
                  if((enable_bbcode || tag.tag_name == "/code") &&
                     !bbcode::closeTag(tag, element))
                      throw(SingleBracketBBCode);
                  break;

                case SingleAngleBracket:
                  if(enable_wikicode || tag.tag_name == "/nowiki")
                  {
                    if(tag.tag_name == "/nowiki")
                    {
                      while(element.nodeName() != "span" && element.attribute("class") == "preformatted")
                        { element.becomeParent(); }
                      element.becomeParent();
                    }
                    else if(element.nodeName() == tag.tag_name.mid(1)) // if the tag name matches the current
                      element.becomeParent(); // become the parent node of the current node
                  }
                  break;

                default: // other types shouldnt have closing tags
                  throw(InvalidClosingTag);
              }
            }
// OPEN TAG
            else
            {
              switch(tag.id)
              {
                case SingleBracketBBCode:
                  if(!enable_bbcode)
                    throw(SingleBracketBBCode);
                  else if(bbcode::simple_pairs.contains(tag.tag_name)) // if simple bbcode tag (with html equivalent)
                    element.becomeChild(tag.tag_name);
                  else if(bbcode::complex_tags.contains(tag.tag_name)) // if complex bbcode needs translating to html
                  {
                    auto attr = tag.attributes.begin();
                    int value;
                    bool ok = true;

                    try
                    {
                      switch(tag.tag_name)
                      {
                        case "code"_hash:
                          element.becomeChild("pre");
                          break;
                        case "break"_hash:
                          element.becomeChild("br");
                          element.becomeParent();
                          break;

                        case "list"_hash:
                          switch(tag.attributes.size())
                          {
                            case 0: // no argument, e.g. [list]
                              element.becomeChild("ul");
                              break;

                            case 1: // one argument, e.g. [list=1]
                              element.becomeChild("ol");
                              attr->value().toInt(&ok);
                              if(ok)
                                element.setAttributes({{"type", "1"}, {"start", attr->value()}});
                              else if(bbcode::romanToInt(attr->value(), value))
                                element.setAttributes({{"type", QChar::isUpper(attr->value().at(0)) ? "I" : "i"}, {"start", value}});
                              else if(bbcode::lettersToInt(attr->value(), value))
                                element.setAttributes({{"type", QChar::isUpper(attr->value().at(0)) ? "A" : "a"}, {"start", value}});
                              else
                                throw("list: exit 1");
                              break;

                            case 2: // two arguments, e.g. [list=A,50]
                              element.becomeChild("ol");
                              if((attr->value()).size() != 1 || !QString("AaIi1").contains(attr->value().at(0)))
                                throw("list: exit 2");
                              element.setAttribute("type", attr->value());
                              ++attr;
                              attr->value().toInt(&ok);
                              if(!ok)
                                throw("list: exit 3");
                              element.setAttribute("start", attr->value());
                              break;

                            default:
                              throw("list: exit 4");
                          }
                          break; // [/list]

                        case "*"_hash:
                          if(m_mode != CParserMode::BBList)
                            throw("*");
                          Q_ASSERT(element.inStack("ol") ||
                                   element.inStack("ul") ||
                                   element.inStack("dl"));

                          while(!html::list_tags.contains(element.nodeName()))
                            { element.becomeParent(); }
                          element.becomeChild("li");
                          break; // [/*]

                        case "table"_hash:
                          element.startTable("bbcode");
                          break;

                        case "td"_hash:
                          if(m_mode != CParserMode::BBTable)
                            throw("td");
                          element.startCell("bbcode");
                          break;

                        case "th"_hash:
                          if(m_mode != CParserMode::BBTable)
                            throw("th");
                          element.startHeader("bbcode");
                          break;

                        case "tr"_hash:
                          if(m_mode != CParserMode::BBTable)
                            throw("tr");
                          element.startRow("bbcode");
                          break;

                        case "caption"_hash:
                          if(m_mode != CParserMode::BBTable)
                            throw("caption");
                          element.startCaption("bbcode");
                          break;

                        case "size"_hash:
                          if(tag.attributes.size() != 1)
                            throw("size: exit 1");

                          switch(HUString(attr->value()))
                          {
                            case "h0"_hash:
                            case "h1"_hash:
                            case "h2"_hash:
                            case "h3"_hash:
                            case "h4"_hash:
                            case "h5"_hash:
                            case "h6"_hash:
                              element.becomeChild(attr->value().toLower()); // make it a header tag
                              break;

                            case "small"_hash:
                            case "smaller"_hash:
                              element.becomeChild("small");
                              break;
                            case "big"_hash:
                            case "bigger"_hash:
                              element.becomeChild("big");
                              break;

                            default:
                              if(attr->value().endsWith("px") ||
                                  attr->value().endsWith("pt") ||
                                  attr->value().endsWith("em"))
                              {
                                QByteArray(attr->value().data(), attr->value().size() - 2).toFloat(&ok);
                                if(!ok)
                                  throw("size: exit 2");
                                element.becomeChild("span", {"style", "font-size:" + attr->value()});
                              }
                              else // must be an integer
                              {
                                attr->value().toInt(&ok);
                                if(!ok)
                                  throw("size: exit 3");
                                element.becomeChild("font", { "size", attr->value() });
                              }
                              break;
                          }
                          break; // [/size]

                        case "quote"_hash:
                          element.becomeChild("blockquote");
                          break;

                        case "rule"_hash:
                          element.becomeChild("hr");
                          switch(tag.attributes.size())
                          {
                            case 1: element.setAttribute("width", attr->value());
                            case 0: break;
                            default: throw("rule");
                          }
                          break; // [/rule]

                        case "color"_hash:
                          if(tag.attributes.size() != 1)
                            throw("color");
                          element.becomeChild("span", {"style", "color:" + attr->value()});
                          break; // [/color]

                        case "highlight"_hash:
                          if(tag.attributes.size() != 1)
                            throw("highlight");
                          element.becomeChild("span", {"style", "background-color:" + attr->value()});
                          break; // [/highlight]

                        case "left"_hash:
                        case "right"_hash:
                        case "center"_hash:
                          element.becomeChild("span", {"style", "float:" + tag.tag_name});
                          break;

                        case "abbr"_hash:
                          if(tag.attributes.size() != 1)
                            throw("abbr");
                          element.becomeChild("abbr", {"title", attr->value()});
                          break;

                        case "o"_hash:
                          element.becomeChild("span", {"class", "overline"});
                          break;

                        case "img"_hash:
                          break;

                        case "url"_hash:
                          break;

                        default:
                          throw(tag.tag_name.data());
                      }
                    }
                    catch(const char* source)
                    {
#ifdef DEBUG_PARSER
                      qDebug() << "bailed out on bbcode tag:" << source;
#endif
                      throw(SingleBracketBBCode); // secondary bail out
                    }
                  }
                  else if(bbcode::table_only.contains(tag.tag_name) && m_mode == CParserMode::BBTable) // part of a bbcode table AND in a bbcode table
                    element.becomeChild(tag.tag_name);
                  else if(bbcode::list_items.contains(tag.tag_name) && m_mode == CParserMode::BBList) // part of a bbcode list AND in a bbcode list
                  {
                    if(bbcode::list_items.contains(element.nodeName())) // currently in list element
                    {
                      element.becomeParent(); //
                      element.becomeChild("li");
                    }
                    else
                      throw(SingleBracketBBCode);
                  }
                  else
                  {
#ifdef DEBUG_PARSER
                      qDebug() << "ignoring tag:" << tag.tag_name;
#endif
                    throw(SingleBracketBBCode);
                  }

                  element.setAttribute("tagtype", "bbcode");
                  break;

                case SingleAngleBracket:
                  if(tag.tag_name == "table")
                    element.startTable("html");
                  else if(html::pairs.contains(tag.tag_name))
                    element.becomeChild(tag.tag_name, {"tagtype", "html"});
                  else if(html::noclose.contains(tag.tag_name))
                    element.appendElement(tag.tag_name);
                  else if(wikicode::formatting_pairs.contains(tag.tag_name)) // special formatting tags
                  {
                    if(tag.tag_name == "nowiki") // special action for <nowiki> tags
                      element.becomeChild("span", {{"tagtype", "html"}, {"class", "preformatted"}});
                  }
                  else if(html::table_only.contains(tag.tag_name) &&
                          m_mode == CParserMode::HtmlTable)
                  {
                    switch(tag.tag_name)
                    {
                      case "td"_hash:      element.startCell   ("html"); break;
                      case "th"_hash:      element.startHeader ("html"); break;
                      case "tr"_hash:      element.endRow      ("html"); break;
                      case "caption"_hash: element.startCaption("html"); break;
                      default:
                        Q_ASSERT(false); // unpossible!
                    }
                  }
                  else if((m_mode == CParserMode::HtmlOrderedList ||
                           m_mode == CParserMode::HtmlUnorderedList) &&
                          (html::list_tags.contains(tag.tag_name) ||
                           html::list_items.contains(tag.tag_name)))
                  {
                    Q_ASSERT(false); // seriously? you manually made a list in html?! oh my!
                    throw(SingleAngleBracket);
                  }
                  else
                    throw(SingleAngleBracket);

                  for(auto pos = tag.attributes.begin(); pos != tag.attributes.end(); ++pos)
                    element.setAttribute(pos->key(), pos->value().replace('"', ' '));
                  break;

                case SingleBracketWikiLink:
                  if(!enable_wikicode)
                    throw(SingleBracketWikiLink);

                  element.becomeChild("a", {{"tagtype", "wiki"}, {"href", tag.tag_namespace + ":" + tag.tag_name}});
                  if(tag.attributes.empty())
                    element.appendText(tag.tag_namespace + ":" + tag.tag_name);
                  else
                  {
                    Q_ASSERT(tag.attributes.size() == 1);
                    element.appendText(tag.attributes.begin()->value());
                  }

                  element.becomeParent();
                  break;

                case DoubleBracket:

                  if(!enable_wikicode)
                    throw(DoubleBracket);

                  if(tag.tag_namespace.isEmpty()) // very simple link
                  {
                    element.becomeChild("a", {{"tagtype", "wiki"}, {"href", encode_link(tag.tag_name)}});
                    if(!page_exists(encoder(tag.tag_name)))
                      element.setAttribute("class", "new");

                    HUString text;
                    switch(tag.attributes.size())
                    {
                      case 0: text = tag.tag_name; break;
                      case 1: text = tag.attributes.begin()->value(); break;
                      default: throw(DoubleBracket);
                    }
                    while(stream.next().isLetter())
                    {
                      ++stream;
                      text.append(stream);
                    }
                    element.appendText(text);
                    element.becomeParent();
                  }
                  else // call template
                  {
                    switch(renderers.value(tag.tag_namespace, "Native"_hash))
                    {
                      case "Native"_hash:
                        switch(tag.tag_namespace)
                        {
                          case "Image"_hash:
                            CImageModule(tag).render(element);
                            break;

                          case "Category"_hash:
                            if(!embedded)
                              m_categories.push_back(tag.tag_name);
                            break;

                          //case "Template"_hash:
                            //CTemplateModule(tag).render(element);
                            //break;

                          default: // default action: build a link!
                          {{
                            // create link
                            HUString link = "/";
                            if(tag.tag_interwiki.isEmpty())
                            {
                              tag.tag_interwiki = tag.tag_namespace;
                              tag.tag_namespace.clear();
                            }

                            if(interwiki_links.contains(tag.tag_interwiki))
                              link.prepend(interwiki_links.value(tag.tag_interwiki));
                            else
                              link.append(encoder(tag.tag_interwiki)).append(":");

                            if(!tag.tag_namespace.isEmpty())
                              link.append(encoder(tag.tag_namespace)).append(":");

                            link.append(encoder(tag.tag_name));
                            // link finished

                            // create text
                            HUString text;
                            if(tag.attributes.isEmpty())
                            {
                              if(!tag.tag_interwiki.isEmpty())
                                text.append(tag.tag_interwiki).append(":");
                              if(!tag.tag_namespace.isEmpty())
                                text.append(tag.tag_namespace).append(":");
                              text.append(tag.tag_name);
                            }
                            else
                              text = tag.attributes.begin()->value();
                            // text finished

                            element.becomeChild("a", {{"tagtype", "wiki"}, {"href", link }});

                            if(tag.tag_namespace.isEmpty() &&
                               !interwiki_links.contains(tag.tag_interwiki) &&
                               !page_exists(nsmap.value(tag.tag_interwiki, ENamespaces::BADVALUE), encoder(tag.tag_name)))
                              element.setAttribute("class", "new");

                              while(stream.next().isLetter())
                              {
                                ++stream;
                                text.append(stream);
                              }
                              element.appendText(text);
                            element.becomeParent();
                            break;
                          }}
                        }
                        break;

                      case "Lua"_hash:
                        {{
                          tag.attributes.emplace_front("", tag.tag_name); // argument #0
                          tag.attributes.emplace_front("function", "embed"); // function to call
                          QDomDocument subdoc;
                          subdoc.setContent(CLuaSandbox().setArguments(tag.tag_namespace, tag.attributes).execScript().replace("&", "&amp;"), false); // run lua script with arguments
                          element.appendChild(subdoc.documentElement());
                        }}
                        break;

                      default:
                        throw(DoubleBracket);
                    }
                  }
                  break;

                case DoubleCurlyBracket: // transclusions/templates
                  if(!enable_wikicode)
                    throw(DoubleCurlyBracket);


                  if(tag.ns_opener) // transclusion
                  {
                    if(lineage.contains(tag.tag_name)) // circular transclusion!
                      throw(DoubleCurlyBracket);
                    lineage.push(encoder(tag.tag_name));
                    element.appendComment("<article name=\"" + lineage.top() + "\" state=\"" + m_mode + "\">");
                    parse(element, lineage, true);
                    element.appendComment("</article name=\"" + lineage.top() + "\" state=\"" + m_mode + "\">");
                    lineage.pop();
                  }
                  else // template
                  {
                    Q_ASSERT(false); // todo
                  }
                  break;

                case TripleCurlyBracket: // template argument
                  if(!enable_wikicode)
                    throw(TripleCurlyBracket);

                  qDebug() << "template argument name/number:" << tag.tag_name;
                  //element.appendText();

                  break;


                case HTMLComment:
                  break;

                default: // this /should/ be an impossible situation
                  throw(UnknownBracketing);
              }
            }
          }
          catch(ETagTypes source)
          {
#ifdef DEBUG_PARSER
            Q_ASSERT(source != UnknownBracketing);
            if(source != InvalidTag)
              qDebug() << "aborted from" << e_tag_types_strings[(int)source];
#endif
            stream.seek(m_start); // rewind
            simple_parse(element, stream);
          }
          break;

        case ECharacter::exclaimation:
          if(m_mode.wikicode() &&
             m_mode == CParserMode::WikiTableLine &&
             stream.next() == ECharacter::exclaimation)
          {
            ++stream;
            element.startHeader("wiki");
            if(stream.next() != ECharacter::endline)
              parse_if_in_table_style(stream, element);
          }
          else
            simple_parse(element, stream);
          break;

        case ECharacter::pipe:
          if(m_mode.wikicode() &&m_mode == CParserMode::WikiTableLine)
          {
            if(stream.next() != ECharacter::pipe)
            {
              ++stream;
              element.startCell("wiki");
              if(stream.next() != ECharacter::endline)
                parse_if_in_table_style(stream, element);
            }
          }
          else
            simple_parse(element, stream);
          break;

        default:
          simple_parse(element, stream);
          break;
      }
    }
  }
}


void CCodeGenerator::setTitle (const HUString& _title)
{
  HUString t = decoder(_title);
  m_edit.setTitle(t);
  CPageGenerator::setTitle(t);
}


void CCodeGenerator::reinit(void)
{
  m_table_depth = 0;
  m_categories.clear();
  m_mode.reset();
  m_toc = createElement("div");
  m_edit = createElement("div");
}

void CCodeGenerator::finish(void)
{
  m_edit.render();
  m_toc.render();

  if(!m_categories.isEmpty())
  {
    m_categories.removeDuplicates();
    CElement f = footer();
    f.becomeChild("div", { {"id", "catlinks"}, {"class", "catlinks"} });
      f.becomeChild("div", { {"id", "mw-normal-catlinks"}, {"class", "mw-normal-catlinks"} });
        f.becomeChild("a", { {"href","/Special:Categories"}, {"title", "Special:Categories"} });
          f.appendText("Category");
        f.becomeParent();
        f.appendText(":");
        f.becomeChild("ul");
        for(auto pos = m_categories.begin(); pos != m_categories.end(); ++pos)
        {
          f.becomeChild("li");
            f.becomeChild("a", { {"href" , HUString("/Category:" + pos->toUtf8()).data()},
                                 {"title", HUString("Category:"  + pos->toUtf8()).data()} });
              f.appendText(*pos);
            f.becomeParent();
          f.becomeParent();
        }
        f.becomeParent();
      f.becomeParent();
    f.becomeParent();
  }
}

void CCodeGenerator::parse(const HUString& _title, const list_t<KVPair>& args)
{
  reinit();
  setTitle(_title);

  for(const KVPair& arg : args)
  {
    qDebug() << "pair: " << arg.first << " " << arg.second;

    if(arg.first == "action")
    {
      switch(arg.second)
      {
        case "preview"_hash:
          m_edit.setPreview(true);
        case "edit"_hash:
          m_edit.setEnabled(true);
          break;
        default:break;
      }
    }
    else if(arg.first == "section")
      m_edit.setSection(arg.second);
    else
    {
      qDebug() << "unknown pair: " << arg.first << " " << arg.second;
    }
  }

  stack_t<HUString> lineage;
  lineage.push(title());

  try
  {
#ifdef DEBUG_SPEED
    auto t = QDateTime::currentMSecsSinceEpoch();
#endif
    CElement b = body();
    parse(b, lineage, false);
#ifdef DEBUG_SPEED
    t = QDateTime::currentMSecsSinceEpoch() - t;
    qDebug() << "Miliseconds elapsed:" << t;
#endif
  }
  catch(QString msg)
  {
    qDebug() << "caught message:" << msg;
#ifdef DEBUG_PARSER
    Q_ASSERT(msg == "ok");
#endif
  }

  finish();
}
