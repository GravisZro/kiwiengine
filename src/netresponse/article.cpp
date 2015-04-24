#include "article.h"

#include <htmlgen/code.h>

bool CArticleResponse::requestFilter(CArticleRequestEvent* request)
{
  CCodeGenerator page;
  page.parse(request->title(), request->arguments());
  page.render(request);
  return false;
}
