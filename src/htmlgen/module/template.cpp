#include "template.h"

CTemplateModule::CTemplateModule(const tag_t& tag)
{
  qDebug() << "template:" << tag.tag_namespace;
  qDebug() << "arg1:" << tag.tag_name;
  for(auto pos = tag.attributes.begin(); pos != tag.attributes.end(); ++pos)
    qDebug() << pos->key() << ":" << pos->value();
}

void CTemplateModule::render(CElement& element)
{
}
