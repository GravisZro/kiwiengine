#ifndef DOMBUILDER_H
#define DOMBUILDER_H

#include <QDomNode>
#include <QVariant>
#include <QDebug>

QDomElement& DomBuilder(QDomElement& current, const QVariant& dom);

typedef QPair<QString, QString> QStringPair;

struct DomElement
{
  QString name;
  QList<QStringPair> attrs;
  QList<DomElement> children;

  void print(void) const;

  DomElement(void) { }
//  DomElement(const QString& n, const QList<QStringPair>& a, const QList<DomElement>& c)
//    : name(n), attrs(a), children(c) { }

//  DomElement(const QString& n, const QStringPair& a, const QList<DomElement>& c)
//    : name(n), children(c) { attrs.push_back(a); }

  DomElement(const QString& n, const QList<QStringPair>& a, const DomElement& c)
    : name(n), attrs(a) { children.push_back(c); }

  DomElement(const QString& n, const QList<QStringPair>& a)
    : name(n), attrs(a) { }

  DomElement(const QString& n, const QStringPair& a, const DomElement& c)
    : name(n) { attrs.push_back(a); children.push_back(c); }

//  DomElement(const QString& n, const QList<DomElement>& c)
//    : name(n), children(c) { }

  DomElement(const QString& n, const QStringPair& a)
    : name(n) { attrs.push_back(a); }

  DomElement(const QString& n, const DomElement& c)
    : name(n) { children.push_back(c); }

  DomElement(const QString& n) // text node!
    : name(n) { }

  //DomElement(const QList<DomElement>& c)
  //  : children(c) { }
};

Q_DECLARE_METATYPE(DomElement);

#endif // DOMBUILDER_H
