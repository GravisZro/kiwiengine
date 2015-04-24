#ifndef ARTICLE_H
#define ARTICLE_H

#include "response_template.h"

class CArticleRequestEvent : public CRequestEventTemplate<Qt::WikiArticle>
{
public:
  inline const HUString& title (void) const { return m_title; }
  inline void setTitle (const HUString& _title) { m_title = _title; }

private:
  HUString m_title;
};

class CArticleResponse : public CResponseTemplate<CArticleRequestEvent, Qt::WikiArticle>
{
public:
  CArticleResponse(QObject* parent = nullptr)
    : CResponseTemplate(parent) { }

  bool requestFilter(CArticleRequestEvent* request);
};

#endif // ARTICLE_H
