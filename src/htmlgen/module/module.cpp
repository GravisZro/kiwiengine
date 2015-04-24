#include "module.h"


module_t::module_t(const tag_t& tag)
{
  qDebug() << "module:" << tag.tag_namespace;
  qDebug() << "arg1:" << tag.tag_name;
  for(auto pos = tag.attributes.begin(); pos != tag.attributes.end(); ++pos)
    qDebug() << pos->key() << ":" << pos->value();
}

void module_t::render(CElement& element)
{
}
