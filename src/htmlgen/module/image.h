#ifndef WIKI_IMAGE_H
#define WIKI_IMAGE_H

#include <globalvars.h>

#include <htmlgen/element.h>
#include <parse/container_stream.h>

struct CImageModule
{
  HUString filename;
// explicit
  //EMedia  image_type;
  bool    exists;
  HUString link;
  HUString alt;
  float   scale; // scale image
  bool    thumbnail;
// inferred
  HUString caption;
  int     width;
  int     height;
  HUString align;
  bool    frame;
  bool    frameless;
  HUString linebreak;
  HUString baseline;

  CImageModule(const tag_t& tag);

  void render(CElement& element);
};

#endif // WIKI_IMAGE_H
