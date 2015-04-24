#include "encode.h"

HUString decoder(const HUString& text)
{
  HUString output = text;
  output.replace(".21", "!")
        .replace(".22", "\"")
        .replace(".24", "$")
        .replace(".25", "%")
        .replace(".27", "'")
        .replace("_", " ");

  return output;
}

HUString encoder(const HUString& text)
{
  auto pos = text.begin();
  auto end = text.end();

  HUString output;
  output.reserve(text.size() * 3);

  for(; pos != end; ++pos)
  {
    switch(*pos)
    {
      case ECharacter::space       : output.push_back(ECharacter::underscore); break;

        // faux percent encoding
      case ECharacter::exclaimation: output.push_back(".21"); break;
      case ECharacter::quotation   : output.push_back(".22"); break;
      case ECharacter::dollar      : output.push_back(".24"); break;
      case ECharacter::percent     : output.push_back(".25"); break;
      case ECharacter::apostrophe  : output.push_back(".27"); break;

        // ignored article title character
      case ECharacter::open_bracket       :
      case ECharacter::close_bracket      :
      case ECharacter::open_anglebracket  :
      case ECharacter::close_anglebracket :
      case ECharacter::pipe               :
      //case ECharacter::hash               :
        break;

      default: output.push_back(*pos);
    }
  }

  return output;
}

HUString encode_link(const HUString& link)
{
  HUString encoded = encoder(link);
  encoded.prepend("/");
  return encoded;
}

HUString image_lookup(const HUString& filename)
{
  HUString encoded = encoder(filename);
  encoded.prepend("/images/");
  return encoded;
}


map_t<hashed_t, HUString> interwiki_links =
{
  { "Wikipedia", "https://en.wikipedia.org/wiki" },
  { "es", "https://es.wikipedia.org/wiki" },
};
