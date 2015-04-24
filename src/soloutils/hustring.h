#ifndef HUSTRING_H
#define HUSTRING_H

#ifdef USE_UNICODE_LIB
#include "basic_type.h"

class UnicodeChar : public basic_type<char16_t>
{
public:
  UnicodeChar(void) { }
  UnicodeChar(const UnicodeChar& other) : basic_type<char16_t>(other) { }

  //bool isPrint          (void) const { return ; }
  inline bool isSpace          (void) const { return value() == u' '; }
  //bool isMark           (void) const { return QChar::isMark(ucs); }
  bool isPunct          (void) const { return ; }
  bool isSymbol         (void) const { return QChar::isSymbol(ucs); }
  bool isLetter         (void) const { return ; }
  bool isNumber         (void) const { return ; }
  bool isLetterOrNumber (void) const { return ; }
  bool isDigit          (void) const { ; }
  bool isLower          (void) const { ; }
  bool isUpper          (void) const { return ; }
  bool isTitleCase      (void) const { return ; }

  //bool isNonCharacter   (void) const { return QChar::isNonCharacter(ucs); }
  //bool isHighSurrogate  (void) const { return QChar::isHighSurrogate(ucs); }
  //bool isLowSurrogate   (void) const { return QChar::isLowSurrogate(ucs); }
  //bool isSurrogate      (void) const { return QChar::isSurrogate(ucs); }
};

#include <unicode/unistr.h>
#include <unicode/uchar.h>

#ifndef complaint
#define complaint(x) HUString(__func__).append(": ").append(x)
#endif



static_assert(sizeof(struct QChar) == 2, "struct exceeds it's designated size");
static_assert(sizeof(UChar) == 2, "incorrect size of unicode character!");

class HUString : public icu::UnicodeString
{
public:
  HUString(void);

  //HUString(const HUString& other);
  HUString(const QString& other);
  HUString(const icu::UnicodeString& other);
  HUString(const char* other) : QString(icu::UnicodeString::fromUTF8(other)) { }

  const UnicodeChar* data(void) const { return reinterpret_cast<const UnicodeChar*>(getBuffer()); }
  operator QString(void) const { return fromUtf16(getBuffer(), size()); }
  int size(void) const { return length(); }

  void clear(void) { icu::UnicodeString::remove(); }

};
#else

//#include <QString>
#include <QVariant>

#include "hashing.h"
#include "containers.h"

#if 1
#include <QByteArray>

class HUString : public QByteArray
{
public:
  inline HUString(void) { }
  inline HUString(const QByteArray& other) : QByteArray(other) { }

  inline HUString(const unsigned int & num) : QByteArray(number(num)) { }
  inline HUString(const          int & num) : QByteArray(number(num)) { }

  inline HUString(const qlonglong & num) : QByteArray(number(num)) { }
  inline HUString(const qulonglong& num) : QByteArray(number(num)) { }

  inline HUString(const char* string8, int length = -1) : QByteArray(string8, length) { }
  inline explicit HUString(const QVariant& other) : QByteArray(other.toByteArray()) { }

  //inline HUString trimmed(void) const { return HUString(QByteArray::trimmed()); }
  //inline HUString mid(int pos, int len = -1) const { return HUString(QByteArray::mid(pos, len)); }
  //inline HUString toLower(void) const { return HUString(QByteArray::toLower()); }

  inline operator const char*(void) const { return data(); }
  inline operator hashed_t(void) const { return hash_array(data(), size()); }
  inline operator int(void) const { return operator hashed_t(); }
};

inline const HUString operator+(const HUString& s1, const HUString& s2) { return operator+(*reinterpret_cast<const QByteArray*>(&s1), *reinterpret_cast<const QByteArray*>(&s2)); }
inline const HUString operator+(const HUString& s1, const char * s2) { return operator+(*reinterpret_cast<const QByteArray*>(&s1), s2); }
inline const HUString operator+(const char* s1, const HUString& s2) { return operator+(s1, *reinterpret_cast<const QByteArray*>(&s2)); }
inline const HUString operator+(char ch, const HUString& s) { return operator+(ch, *reinterpret_cast<const QByteArray*>(&s)); }
inline const HUString operator+(const HUString& s, char ch) { return operator+(*reinterpret_cast<const QByteArray*>(&s), ch); }

#endif

typedef pair_t<HUString, HUString> KVPair;

class HUStringList : public list_t<HUString>
{
public:
  HUStringList(void) { }
  //template <typename... Args> HUStringList(Args... args) : list_t<HUString>({args...}) { }
  HUStringList(std::initializer_list<HUString> values) : list_t<HUString>(values) { }
};

#endif


#endif // HUSTRING_H
