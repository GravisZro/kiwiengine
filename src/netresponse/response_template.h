#ifndef GENERATORTEMPLATE_H
#define GENERATORTEMPLATE_H

#include <QtCore>

#include <util/dbinterface.h>
#include <httpio/request_template.h>


template<class RequestType, quint32 RequestName>
class CResponseTemplate : public QObject
{
public:
  CResponseTemplate(QObject* parent = nullptr) : QObject(parent) { }

  virtual bool requestFilter(RequestType* request) = 0;

  bool eventFilter(QObject* obj, QEvent* event)
  {
    if(event->type() == RequestName)
      return requestFilter(static_cast<RequestType*>(event));
    return QObject::eventFilter(obj, event);
  }
};

#endif // GENERATORTEMPLATE_H
