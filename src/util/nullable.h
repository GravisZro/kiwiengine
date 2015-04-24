#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstddef>

template<typename T,
         typename t0 = T,
         typename t1 = T,
         typename t2 = T,
         typename t3 = T,
         typename t4 = T,
         typename t5 = T,
         typename t6 = T,
         typename t7 = T,
         typename t8 = T,
         typename t9 = T>
class nullable : T
{
public:
  nullable(void) : T(), m_null(nullptr) { }
  nullable(t0 v0) : T(v0), m_null(1) { }
  nullable(t0 v0, t1 v1) : T(v0, v1), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2) : T(v0, v1, v2), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3) : T(v0, v1, v2, v3), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3, t4 v4) : T(v0, v1, v2, v3, v4), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) : T(v0, v1, v2, v3, v4, v5), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6) : T(v0, v1, v2, v3, v4, v5, v6), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7) : T(v0, v1, v2, v3, v4, v5, v6, v7), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8) : T(v0, v1, v2, v3, v4, v5, v6, v7, v8), m_null(1) { }
  nullable(t0 v0, t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6, t7 v7, t8 v8, t9 v9) : T(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9), m_null(1) { }

  const std::nullptr_t& operator ==(const std::nullptr_t n) { return n == m_null; }

  t0& operator =(const t0& v)
  {
    m_null = nullptr;
    return T::operator =(v);
  }

private:
  void* m_null;
};

template<typename T>
class basic_type
{
public:
  T& operator =(const T& value) { return m_value = value; }
  operator T(void) { return m_value; }
private:
  T m_value;
};

// make all the basic types usable with nullable

template<> class nullable<char                  > : public nullable<basic_type<char                  > > { };
template<> class nullable<int                   > : public nullable<basic_type<int                   > > { };
template<> class nullable<short                 > : public nullable<basic_type<short                 > > { };
template<> class nullable<long int              > : public nullable<basic_type<long int              > > { };
template<> class nullable<long long int         > : public nullable<basic_type<long long int         > > { };

template<> class nullable<unsigned char         > : public nullable<basic_type<unsigned char         > > { };
template<> class nullable<unsigned int          > : public nullable<basic_type<unsigned int          > > { };
template<> class nullable<unsigned short        > : public nullable<basic_type<unsigned short        > > { };
template<> class nullable<long unsigned int     > : public nullable<basic_type<long unsigned int     > > { };
template<> class nullable<long long unsigned int> : public nullable<basic_type<long long unsigned int> > { };

template<> class nullable<float                 > : public nullable<basic_type<float                 > > { };
template<> class nullable<double                > : public nullable<basic_type<double                > > { };
template<> class nullable<long double           > : public nullable<basic_type<long double           > > { };


#endif // GLOBAL_H
