#ifndef WIKIDBINTERFACE_H
#define WIKIDBINTERFACE_H

#include <soloutils>
#include "queries.h"
#include "dbinterface.h"

HUString latest_page(const HUString& name);
HUString latest_page(ENamespaces ns, const HUString& name);
bool page_exists(const HUString& name);
bool page_exists(ENamespaces ns, const HUString& name);

vector_t<QVariant> media_info(const HUString& name);

#endif // WIKIDBINTERFACE_H
