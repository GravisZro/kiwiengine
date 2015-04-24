#if 0
//#ifndef USTRING_H
#define USTRING_H

//#include <unicode/ustring.h>
//#include <unicode/unistr.h>

#include <stdint.h>
#include "hashing.h"
#include <cstring>
#include "containers.h"

#include <string>
#include <array>
#include <vector>
#include <regex>


template<typename ptr_t> constexpr ptr_t ptrinc(ptr_t ptr)
{ return ptr == nullptr ? nullptr : ptr + 1; }


constexpr size_t real_const_strlen(const char* data)
{ return !data[0] ? 0 : 1 + real_const_strlen(data + 1); }

constexpr size_t const_strlen(const char* data)
{ return data == nullptr ? 0 : real_const_strlen(data); }

struct strdata_t
{
  const char* data;
  size_t length;

  inline bool operator == (const char* other) const
    { return !std::memcmp(data, other, length); }

  inline bool operator == (const strdata_t& other) const
    { return length == other.length && !std::memcmp(data, other.data, length); }

           constexpr strdata_t(const char* d, size_t l      ) : data(d), length(l) { }
  explicit constexpr strdata_t(const char* d                ) : data(d), length(const_strlen(d)) { }
  explicit constexpr strdata_t(const char* d, const char* d2) : data(d), length(d2 - d) { }
};

template<char target> constexpr bool is   (char data) { return data == target; }
template<char target> constexpr bool isNot(char data) { return data != target; }

constexpr bool isDigit (char data) { return data >= '0' && data <= '9'; }
constexpr bool isLetter(char data) { return (data >= 'a' && data <= 'z') || (data >= 'A' && data <= 'Z'); }
constexpr bool isLetterOrDigit(char data) { return isDigit(data) || isLetter(data); }

constexpr size_t lengthOfDashes(const char* data)
{ return data[0] != '-' ? 0 : (1 + lengthOfDashes(ptrinc(data))); }

typedef bool (*test_func)(char);
template<test_func function> constexpr const char* rseek(const char* data)
{ return !data[0] ? nullptr : (function(data[0]) ? rseek<function>(data + 1) : data); }

struct option_t
{
  const bool is_long;
  const strdata_t flag_name;
  const strdata_t value_name;
  const strdata_t value_pattern;
  const strdata_t description;

  constexpr option_t(const char* data, const char* desc)
    : is_long(lengthOfDashes(data) == 2),
      flag_name    (       rseek<is   <'-'>>(data), rseek<isLetterOrDigit>(data + lengthOfDashes(data))),
      value_name   (ptrinc(rseek<isNot<'<'>>(data)), rseek<isNot<'>'>>(data)),
      value_pattern(ptrinc(rseek<isNot<'>'>>(data))),
      description(desc)
  { }
};

struct argument_t
{
  strdata_t name;
  strdata_t value;
};

const std::list<option_t> options =
//option_t const options[] =
{
  { "-f", "short flag" },
  { "-o <description>(regex)", "old option" },
  { "-o <description>[number:-1,200]", "old option" },
  { "-o <description>[decimal:-1.0,200.0]", "old option" },
  { "-o <description>[string]", "old option" },

  { "-o <description>[FILE:rw]", "old option file" },
  { "-o <description>[DEVICE:r]", "old option device file" },
  { "-o <description>[URI:http,https]", "old option uri" },

  { "--flag", "long flag" },
  { "--opt=<description>(regex)", "new option regex" },
  { "--opt=<description>[number:-1,200]", "new option" },
  { "--opt=<description>[decimal:-1.0,200.0]", "new option" },
  { "--opt=<description>[string]", "new option" },
  { "--opt=<description>[FILE]", "new option file" },
  { "--opt=<description>[DEVICE]", "new option device file" },
  { "--opt=<description>[URI]", "new option uri" },
};

std::map<strdata_t, variant_t> parseOptions(int argc, char **argv)
{
  std::map<strdata_t, variant_t> opts;
  for(int i = 1; i < argc; ++i)
  {
    struct argument_t arg;
    const char* begin = argv[i];
    const char* end = std::strlen(argn);
    const char* argn = begin;

    while(*argn == '-') { ++argn; }
    arg.name.data = argn;
    while(isLetterOrDigit(*argn)) { ++argn; }
    arg.name.length = argn - arg.name.data;

    if(opts.find(arg.name) != opts.end())
      throw("duplicate option");

//    for(auto opt : options)
//      if()

    if(argn < end)
    {

    }


    arg.name.data = argn + lengthOfDashes(argn);
    arg.name.length = rseek<isLetterOrDigit>(data + lengthOfDashes(data));
    for(int i = 0; i < length; ++i)

  }
}


#endif // USTRING_H
