#ifndef HUTILS_H
#define HUTILS_H

#include <list>
#include <initializer_list>
#include <map>
#include <vector>

#if 0

template<typename T, typename U>
class pair_t
{
public:
  pair_t(void) { }
  pair_t(const T& k, const U& v) : first(k), second(v) { }

  inline T& key  (void) { return first ; }
  inline U& value(void) { return second; }

  const T& key  (void) const { return first ; }
  inline const U& value(void) const { return second; }

private:
  T first;
  U second;
};
#endif

#if 0
#include <QPair>

template<typename T, typename U>
struct pair_t : QPair<T, U>
{
  inline pair_t(void) { }
  inline pair_t(const T& k, const U& v) : QPair<T, U>(k, v) { }

  inline T& key  (void) { return QPair<T, U>::first ; }
  inline U& value(void) { return QPair<T, U>::second; }

  inline const T& key  (void) const { return QPair<T, U>::first ; }
  inline const U& value(void) const { return QPair<T, U>::second; }
};
#else
#include <utility>

template<typename T, typename U>
struct pair_t : std::pair<T, U>
{
  inline pair_t(void) { }
  inline pair_t(const pair_t<T, U>& other) : std::pair<T, U>(other) { }
  inline pair_t(const T& k, const U& v) : std::pair<T, U>(k, v) { }

  inline T& key  (void) { return std::pair<T, U>::first ; }
  inline U& value(void) { return std::pair<T, U>::second; }

  inline const T& key  (void) const { return std::pair<T, U>::first ; }
  inline const U& value(void) const { return std::pair<T, U>::second; }
};
#endif

#if 1
#include <QList>
template<typename T>
struct list_t : QList<T>
{
  inline list_t(void) { }
  inline list_t(std::initializer_list<T> values) : QList<T>(values) { }
  inline list_t(list_t& other) : QList<T>(other) { }
  inline list_t(const T& single) { push_back(single); }
  //template<typename... Params> inline list_t(Params... parameters) : QList<T>(parameters...) { }


  template<typename... Params> inline void emplace_back (Params... parameters) { QList<T>::push_back (T(parameters...)); }
  template<typename... Params> inline void emplace_front(Params... parameters) { QList<T>::push_front(T(parameters...)); }

  //inline void removeDuplicates(void) { list_t<T>::unique(); }
};

#else
#include <list>
template<typename T>
struct list_t : std::list<T>
{
  inline list_t(void) { }
  inline list_t(std::initializer_list<T> values) : std::list<T>(values) { }

  inline bool isEmpty(void) const { return list_t<T>::empty(); }
  inline void removeDuplicates(void) { list_t<T>::unique(); }
  bool contains(const T& target) const
  {
    for(auto pos = std::list<T>::begin(); pos != std::list<T>::end(); ++pos)
      if(*pos == target)
        return true;
    return false;
  }
};
#endif

#if 1
#include <QMap>

template<typename T, typename U>
struct map_t : QMap<T, U>
{
  inline map_t(void) { }
  inline map_t(std::initializer_list<std::pair<const T, U>> values) : QMap<T, U>(values) { }
};

#else
#include <map>
template<typename T, typename U>
struct map_t : std::map<T, U>
{
  inline map_t(void) { }
  inline map_t(std::initializer_list<std::pair<const T, U>> values) : std::map<T, U>(values) { }

  template<typename rtype>
  inline rtype insert(const T& key, const U& val) { return std::map<T, U>::emplace(key, val); }

  inline bool isEmpty(void) const { return std::map<T, U>::empty(); }
  bool contains(const T& target) const { return std::map<T, U>::find(target) != std::map<T, U>::end(); }
};
#endif

#if 1
#include <QStack>
template<typename T>
struct stack_t : QStack<T>
{
  inline stack_t(void) { }
  inline stack_t(std::initializer_list<T> values) : QStack<T>(values) { }
};
#else
// fake stack
template<typename T>
struct stack_t : list_t<T>
{
  inline stack_t(void) { }
  inline stack_t(std::initializer_list<T> values) : list_t<T>(values) { }

  inline bool isEmpty(void) const { return list_t<T>::empty(); }
  inline T& top(void) { return list_t<T>::back(); }
  inline const T& top(void) const { return list_t<T>::back(); }
  inline void push(const T& value) { list_t<T>::push_back(value); }
  inline void pop(void) { list_t<T>::pop_back(); }

  inline int count(const T& target) const
  {
    int num = 0;
    for(auto pos = std::list<T>::begin(); pos != std::list<T>::end(); ++pos)
      if(*pos == target)
        ++num;
    return num;
  }
};
#endif

#if 1
#include <QVector>

template<typename T>
struct vector_t : QVector<T>
{
  inline vector_t(void) { }
  inline vector_t(std::initializer_list<T> values) : QVector<T>(values) { }

  template<typename... Params> inline void emplace_back (Params... parameters) { QVector<T>::push_back (T(parameters...)); }
  template<typename... Params> inline void emplace_front(Params... parameters) { QVector<T>::push_front(T(parameters...)); }
};
#else
template<typename T>
struct vector_t : std::vector<T>
{
  inline vector_t(void) { }
  inline vector_t(std::initializer_list<T> values) : std::vector<T>(values) { }

  inline bool isEmpty(void) const { return std::vector<T>::empty(); }
};
#endif


#endif // HUTILS_H
