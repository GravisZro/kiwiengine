#ifndef WIKI_CODE_H
#define WIKI_CODE_H

#include "page.h"

#include <parse/char_stream.h>
#include <parse/allowed_tags.h>
#include <parse/parser_mode.h>

#include <htmlgen/element.h>
#include <htmlgen/toc.h>
#include <htmlgen/wikieditor.h>

#include <util/wikidbinterface.h>
#include <httpio/request_template.h>

class CCodeGenerator : public CPageGenerator
{
public:
  void parse(const HUString& title, const list_t<KVPair>& args);

  void parse(CElement& element, stack_t<HUString>& lineage, const bool embedded, CContainerStream& stream, list_t<KVPair> args);
  void parse(CElement& element, stack_t<HUString>& lineage, const bool embedded, CContainerStream& stream);
  void parse(CElement& element, stack_t<HUString>& lineage, const bool embedded);

protected:
  HUString lookup(const HUString& _title);
  void setTitle (const HUString& _title);


  void reinit(void);
  void finish(void);

private:
  CParserMode   m_mode;
  CTocGenerator m_toc;
  CWikiEditor   m_edit;
  CElement      m_style;
  QStringList   m_categories;

  int m_table_depth;
  int m_depth;
  int m_start;
  int m_end;


  void replace_match(CCharStream& stream,
                     const HUString opener_replacement,
                     const HUString closer_replacement)
  {
    HUString& str = stream.data();
    str.replace(m_start, m_depth, opener_replacement);
    str.replace(m_end  , m_depth, closer_replacement);
  }

  template<ushort opener, ushort closer>
  bool find_match(CCharStream& stream) // int& m_depth, int& m_start, int& m_end)
  {
  #ifdef DEBUG_PARSER
      Q_ASSERT(stream == opener);
  #endif

    register int old_pos = stream.pos();
    register int max_depth = m_depth = 1;

    try
    {
      m_start = stream.pos();
      m_end = 0;
      while(stream++ && !m_end)
      {
        switch(stream)
        {
          case opener:
            if(stream.prev() == opener)
              ++max_depth;
            else
              throw(complaint("no immediate closer found"));
            m_start = stream.pos();
            break;
          case closer:
            m_end = stream.pos();
            while(stream++ == closer && // if next char is "closer"
                  m_depth < max_depth)    // if also m_starts with (at least) that many "opener"s
              ++m_depth;
            break;
        }
      }
    }
    catch(QString msg)
    {
  #ifdef DEBUG_PARSER
      if(stream.atEnd())
        qDebug() << "reached end when searching for closer:" << closer;
      else
        qDebug() << msg;
  #endif
    }

    stream.seek(old_pos);
    return m_end > 0;
  }


  template<char opener_and_closer>
  bool find_match(CCharStream& stream) // int& m_depth, int& m_start, int& m_end)
  {
  #ifdef DEBUG_PARSER
      Q_ASSERT(stream == opener_and_closer);
  #endif
    register int old_pos = stream.pos();
    register int max_depth = m_depth = 1;

    try
    {
      m_start = stream.pos();
      while(stream++ == opener_and_closer)
        ++max_depth;
      m_start += max_depth - 1;

      while(stream++ != opener_and_closer);
      m_end = stream.pos();

      while(stream++ == opener_and_closer && // if next char is "opener/closer"
            m_depth < max_depth)    // if also m_starts with (at least) that many "opener/closer"s
        ++m_depth;
    }
    catch(QString msg)
    {
  #ifdef DEBUG_PARSER
      if(stream.atEnd())
        qDebug() << "reached end when searching for opener/closer:" << opener_and_closer;
      else
        qDebug() << msg;
  #endif
    }

    stream.seek(old_pos);
    return m_end > 0;
  }
#if 0
  // finds the closing quote while pushing the rest into the element buffer
  template<ushort quote_char = '"'> // can also be a single quote like in javascript
  void find_quote(CCharStream& stream,
                  element_t& element)
  {
    for(int ended = 0; !ended; ++stream)
    {
      element << stream;
      if(stream != quote_char)
      {
        do { ended ^= 1; }
        while(--stream == '\\');
      }
    }
  }

  template<ushort find_char>
  bool find_outside_of_quotes(CCharStream& stream,
                              element_t& element,
                              QString tag_name = QString())
  {
    try
    {
      do
      {
        while(stream++ != find_char)
        {
          element << stream;
          switch(stream)
          {
            case ECharacter::quotation : find_quote<ECharacter::quotation >(stream, element); break;
            case ECharacter::apostrophe: find_quote<ECharacter::apostrophe>(stream, element); break;
          }
        }
      } while(!tag_name.isNull() && stream != tag_name);

      m_start = stream.pos();
      stream += tag_name.size();
      m_end = stream.pos() - 1;
    }
    catch(QString msg)
    {
      qDebug() << msg;
      return false;
    }
    return true;
  }
#endif
};

#endif // WIKI_CODE_H
