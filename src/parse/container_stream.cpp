#include "container_stream.h"

struct tag_desc_t // contents can _ONLY_ be set in initializer list
{
  const HUString opener;
  const int open_depth;

  const HUString closer;
  const int close_depth;

  const bool can_abort;

  const char ns_delim;
  const char delim;
  const char delim2;

  const char attr_delim;
  const char attr_delim2;
  const char attr_value_delim;

  const bool is_wikicode;
  const bool respects_quotes; // tag cant end until quotes are closed

  inline bool no_abort(const CCharStream& stream) const
  {
    return can_abort &&
           stream != closer &&
           (is_wikicode || opener[0] != stream); // wikicode can contain opener[0]
  }
};

// note: the order of "taggers" is important!
const tag_desc_t taggers[] = { { "["   , 1, "]"  , 1, true ,   0, '=',    0, ',',    0,   0, false, true  }, // bbcode
                               { "["   , 1, "]"  , 1, true , ':', ' ',    0,   0,    0,   0, true , false }, // global link
                               { "<"   , 1, ">"  , 1, true ,   0, ' ', '\n', ' ', '\n', '=', false, true  }, // html/special wikicode
                               { "[["  , 2, "]]" , 2, true , ':', '|',    0, '|',    0, '=', true , false }, // wiki link
                               { "{{"  , 2, "}}" , 2, true , ':', '|',    0, '|',    0, '=', true , false }, // template
                               { "{{{" , 3, "}}}", 3, true , ':', '|',    0, '|',    0, '=', true , false }, // template argument
                               { "<!--", 4, "-->", 3, false,   0,   0,    0,   0,    0,   0, false, false }, // html comment
                             };

static ETagTypes id_tag_opener(CCharStream& stream)
{
  for(int pos = (sizeof(taggers) / sizeof(tag_desc_t)) - 1; pos >= 0; --pos)
    if(stream == taggers[pos].opener)
      return static_cast<ETagTypes>(pos);
  return UnknownBracketing;
}

static ETagTypes id_tag_closer(CCharStream& stream)
{
  for(int pos = (sizeof(taggers) / sizeof(tag_desc_t)) - 1; pos >= 0; --pos)
    if(stream == taggers[pos].closer)
      return static_cast<ETagTypes>(pos);
  return UnknownBracketing;
}

bool CContainerStream::readTag(tag_t& tag, CParserMode& mode)
{
  CCharStream& stream = *this;
  int old_pos = stream.pos();

  try
  {
    tag.opener.clear();
    tag.tag_interwiki.clear();
    tag.tag_name.clear();
    tag.tag_namespace.clear();
    tag.attributes.clear();
    //tag.attribute_list.clear();
    tag.closer.clear();

// parse tag opening and name
    tag.id = id_tag_opener(stream);

    if(tag.id == UnknownBracketing)
      throw(complaint("unknown opening tag(!)"));

    const tag_desc_t* tagger = taggers + tag.id;

    int pos = stream.pos();
    stream += tagger->open_depth;
    tag.opener = HUString(stream.dataAt(pos), tagger->open_depth);

    tag.ns_opener = stream == ':';
    if(tag.ns_opener)
      ++stream;

    pos = stream.pos();

    while(!stream.atEnd() &&
          tagger->ns_delim != stream &&
          tagger->delim    != stream &&
          tagger->delim2   != stream &&
          tagger->no_abort(stream))
      { ++stream; }

    if(stream == tagger->ns_delim)
    {
      tag.tag_namespace = HUString(stream.dataAt(pos), stream.pos() - pos);
      ++stream;
#ifdef DEBUG_TAG_PARSER
      if(tag.id == SingleBracketWikiLink)
        qDebug() << "ns found:" << tag.tag_namespace;
#endif
    }
    else
    {
      if(tag.id == SingleBracketWikiLink)
        tag.id = SingleBracketBBCode;
      stream.seek(pos);
    }

    tagger = taggers + tag.id;
    pos = stream.pos();

    while(!stream.atEnd() &&
          tagger->ns_delim != stream &&
          tagger->delim    != stream &&
          tagger->delim2   != stream &&
          tagger->no_abort(stream))
      { ++stream; }

    if(!stream.atEnd() &&
       tagger->ns_delim == stream)
    {
      tag.tag_interwiki = tag.tag_namespace;
      tag.tag_namespace = HUString(stream.dataAt(pos), stream.pos() - pos).trimmed(); // remove any whitespace at the end
      ++stream;
      pos = stream.pos();

      while(!stream.atEnd() &&
            tagger->ns_delim != stream &&
            tagger->delim    != stream &&
            tagger->delim2   != stream &&
            tagger->no_abort(stream))
        { ++stream; }
    }

    tag.tag_name = HUString(stream.dataAt(pos), stream.pos() - pos).trimmed(); // remove any whitespace at the end
    pos = stream.pos();

// parse tag params

    if((tagger->delim == stream || tagger->delim2 == stream) && ++stream)
    {
      HUString attr_name;
      HUString attr_value;
      do
      {
        while(!stream.atEnd() && stream.isSpace() && ++stream); // find start of attribute
        pos = stream.pos(); // starting position of attribute name

        // find the end of the attribute name
        while(!stream.atEnd() &&
              tagger->attr_delim       != stream &&
              tagger->attr_delim2      != stream &&
              tagger->attr_value_delim != stream &&
              tagger->no_abort(stream))
          { ++stream; }

        attr_name = HUString(stream.dataAt(pos), stream.pos() - pos).trimmed(); // record attibute name

        if(tagger->attr_value_delim == stream) // if there is a value for this attribute...
        {
          // find end of attribute value
          pos = stream.pos() + 1;
          while(!stream.atEnd() &&
                tagger->attr_delim  != stream &&
                tagger->attr_delim2 != stream &&
                tagger->no_abort(stream))
            { ++stream; }

          attr_value = HUString(stream.dataAt(pos), stream.pos() - pos).trimmed(); // record attibute value

          tag.attributes.emplace_back(attr_name, attr_value); // store the name and value in an associative array
        }
        else
        {
          tag.attributes.emplace_back("", attr_name); // only has a value so store the it's arg number and value
        }

        pos = stream.pos();
      } while(!stream.atEnd() &&
              (tagger->attr_delim == stream ||
               tagger->attr_delim2 == stream) && ++stream);
    }

// parse tag closer
    ETagTypes id_closer = id_tag_closer(stream);

    if(id_closer == UnknownBracketing)
    {
#if DEBUG_TAG_PARSER
      qDebug() << "DEBUG:" << stream.operator char();
      qDebug() << "DEBUG:" << (int)stream.operator char();
      qDebug() << "DEBUG:" << QString(stream.dataAt(old_pos), pos - old_pos);
#endif
      throw(complaint("unknown closing tag(!)"));
    }

    if(tag.id > id_closer) // end doesnt match the beginning!
      throw(complaint("opener/closer mismatch!"));

    tag.closer = HUString(stream.dataAt(pos), tagger->close_depth);
    stream += tagger->close_depth - 1;

// manage parser modifying tags (e.g. <nowiki>)
    mode = tag.opener + tag.tag_name + tag.closer;

// fin!
#ifdef DEBUG_TAG_PARSER
    qDebug() << "tag:" << tag.opener << tag.tag_namespace << tag.tag_name << tag.attributes << tag.attribute_list << tag.closer;
#endif
  }
  catch(QString msg)
  {
#ifdef DEBUG_TAG_PARSER
    qDebug() << msg;
#endif
    stream.seek(old_pos);
    return false;
  }

  return true;
}
