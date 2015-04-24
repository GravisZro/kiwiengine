#include "wikidbinterface.h"

HUString latest_page(const HUString& name)
{
  auto result = QueryThread::fastQuery(query("latest page"), name.mid(0, name.indexOf('#')));
  if(result.isEmpty())
    return HUString();
  return result[0][2].toByteArray();
}

HUString latest_page(ENamespaces ns, const HUString& name)
{
  auto result = QueryThread::fastQuery(query("latest page in namespace"), static_cast<int>(ns), name.mid(0, name.indexOf('#')));
  if(result.isEmpty())
    return HUString();
  return result[0][2].toByteArray();
}

bool page_exists(const HUString& name)
{
  return !QueryThread::fastQuery(query("check page exists"), name.mid(0, name.indexOf('#'))).isEmpty();
}

bool page_exists(ENamespaces ns, const HUString& name)
{
  if(ns == ENamespaces::BADVALUE)
    return false;
  return !QueryThread::fastQuery(query("check page exists in namespace"), static_cast<int>(ns), name.mid(0, name.indexOf('#'))).isEmpty();
}

vector_t<QVariant> media_info(const HUString& name)
{
  auto result = QueryThread::fastQuery(query("media info"), name);
  if(result.isEmpty())
    return vector_t<QVariant>();
  return result[0];
}
