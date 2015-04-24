#ifndef WIKI_ENCODE_H
#define WIKI_ENCODE_H

#include <globalvars.h>

#include <soloutils>

namespace ECharacter
{
  const char open_curlybracket  = '{';
  const char close_curlybracket = '}';

  const char open_anglebracket  = '<';
  const char close_anglebracket = '>';

  const char open_bracket  = '[';
  const char close_bracket = ']';

  const char null         = '\0';
  const char slash        = '/';
  const char pipe         = '|';
  const char dash         = '-';
  const char plus         = '+';
  const char exclaimation = '!';
  const char endline      = '\n';
  const char equals_sign  = '=';
  const char quotation    = '"';
  const char apostrophe   = '\'';

  const char space        = ' ';
  const char tab          = '\t';

  const char ampersand    = '&';
  const char asterisk     = '*';
  const char hash         = '#';
  const char percent      = '%';
  const char dollar       = '$';
  const char underscore   = '_';

  const char colon        = ':';
  const char semicolon    = ';';
}

HUString decoder(const HUString& text);
HUString encoder(const HUString& text);
HUString encode_link(const HUString& link);
HUString image_lookup(const HUString& filename);

extern map_t<hashed_t, HUString> interwiki_links;

#endif // WIKI_ENCODE_H
