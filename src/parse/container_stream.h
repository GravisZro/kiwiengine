#ifndef CONTAINERSTREAM_H
#define CONTAINERSTREAM_H

#include <soloutils>

#include "char_stream.h"
#include "parser_mode.h"

enum ETagTypes : int
{
  SingleBracketBBCode = 0,
  SingleBracketWikiLink,
  SingleAngleBracket,
  DoubleBracket,
  DoubleCurlyBracket,
  TripleCurlyBracket,
  HTMLComment,
  UnknownBracketing,
  InvalidTag,
  InvalidClosingTag,
};

const char* const e_tag_types_strings[] =
{
  "SingleBracketBBCode",
  "SingleBracketWikiLink",
  "SingleAngleBracket",
  "DoubleBracket",
  "DoubleCurlyBracket",
  "TripleCurlyBracket",
  "HTMLComment",
  "UnknownBracketing",
  "InvalidTag",
  "InvalidClosingTag",
};

struct tag_t
{
  ETagTypes id;
  HUString opener;
  bool     ns_opener;
  HUString tag_interwiki;
  HUString tag_namespace;
  HUString tag_name;
  list_t<KVPair> attributes;
  HUString closer;
};

const QStringList supported_schemes =
  {
    "http",
    "https",
    "ftp",
    "sftp",
    "mailto",
    "magnet",
    "file",
    "ssh",
    "irc",
    "ircs",
    "xmmp",
    "sip",
    "sips",
    "gopher",
    "telnet",
    "nntp",
    "worldwind",
    "tel",
    "sms",
    "news",
    "svn",
    "git",
    "mms",
    "bitcoin",
    "urn",
    "geo",
  };

// CContainerStream is for parsing string streams continaing html/wikicode/bbcode tags
class CContainerStream : public CCharStream
{
public:
  CContainerStream(const HUString& data) : CCharStream(data) { }
  CContainerStream(const char* data, int length) : CCharStream(data, length) { }
  CContainerStream(void) : CCharStream() { }

  // fills a referenced tag and returns true but
  // returns false if the current position is not a (valid) tag
  bool readTag(tag_t& tag, CParserMode& mode);
};
#endif // CONTAINERSTREAM_H
