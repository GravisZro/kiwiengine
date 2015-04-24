#ifndef WIKI_TEMPLATE_H
#define WIKI_TEMPLATE_H

#include <globalvars.h>

#include <htmlgen/element.h>
#include <parse/container_stream.h>

struct CTemplateModule
{
  CTemplateModule(const tag_t& tag);

  void render(CElement& element);
};


#endif // WIKI_TEMPLATE_H
