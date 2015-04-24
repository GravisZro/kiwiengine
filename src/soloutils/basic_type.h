#ifndef BASIC_TYPE_H
#define BASIC_TYPE_H

template<typename T>
class basic_type
{
public:
  basic_type(void) { }
  basic_type(const T& other) : data(other) { }

  inline T& operator =(const T& value) { return data = value; }

  inline T& value(void) { return data; }
  inline const T& value(void) const { return data; }

  inline operator T&(void) { return value(); }
  inline operator const T&(void) { return value(); }

private:
  T data;
};

#endif // BASIC_TYPE_H
