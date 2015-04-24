#ifndef HASHING_H
#define HASHING_H

#include <cstddef>

/*
template<typename T> constexpr unsigned int hash_string(const T* key, uint seed = 0)
 { return uint(((key >> (8 * sizeof(uint) - 1)) ^ key) & (~0U)) ^ seed; }
static_assert(sizeof(uint) < sizeof(ulong), "whoa, weird architecture!");
*/

template<typename T> constexpr unsigned int hash_word(const T* key)
{ return *key != '\0' &&
         *key != ' '  &&
         *key != '\n' &&
         *key != '\r' ? static_cast<unsigned int>(*key) + 33 * hash_word(key + 1) : 5381; }

template<typename T> constexpr unsigned int hash_string(const T* key)
 { return *key ? static_cast<unsigned int>(*key) + 33 * hash_string(key + 1) : 5381; }

template<typename T> constexpr unsigned int hash_array(const T* key, std::size_t size)
 { return size ? static_cast<unsigned int>(*key) + 33 * hash_array(key + 1, size - 1) : 5381; }

constexpr unsigned int operator "" _hash(const char16_t* s, std::size_t sz) { return hash_array(s, sz); }
constexpr unsigned int operator "" _hash(const char* s, std::size_t sz) { return hash_array(s, sz); }


struct hashed_t
{
  unsigned int value;

  inline operator unsigned int(void) const { return value; }
  inline unsigned int& operator =(const unsigned int& other) { return value = other; }

  inline hashed_t(void) : value(0) { }
  inline hashed_t(const unsigned int& other) : value(other) { }
  inline hashed_t(const int& other) : hashed_t((unsigned int)other) { }
  inline hashed_t(const char* str) : hashed_t(hash_string(str)) { }
//  inline hashed_t(const char16_t* str) : hashed_t(hash_string(str)) { }
#ifdef QBYTEARRAY_H
  inline hashed_t(const QByteArray& arr) : hashed_t(hash_array(arr.data(), arr.size())) { }
#endif

//  static_assert(sizeof(unsigned short) == sizeof(char16_t), "size mismatch, what kind of crazy architecture are you using?!");
};


#endif // HASHING_H
