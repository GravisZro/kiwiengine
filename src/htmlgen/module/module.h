#ifndef WIKI_MODULE_H
#define WIKI_MODULE_H

#include <globalvars.h>

#include <htmlgen/element.h>
#include <parse/container_stream.h>

struct module_t
{
  module_t(const tag_t& tag);

  void render(CElement& element);
};

#endif // WIKI_MODULE_H
