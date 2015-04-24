#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <soloutils>

#include <htmlgen/encode.h>

class CCharStream
{
public:
  CCharStream(void);

  inline CCharStream(const HUString& data)
    : CCharStream() { setData(data); }

  inline CCharStream(const char* data, int length)
    : CCharStream(HUString(data, length)) { }


  CCharStream& operator ++(void);
  CCharStream& operator --(void);
  CCharStream& operator +=(int length);

  bool operator ==(const HUString& other) const;
  inline bool operator !=(const HUString& other) const { return !operator ==(other); }

  void setData(const HUString& data);

  int indexInLine(const char& target) const;
  inline int indexOf(const char& target) const { return m_data.indexOf(target, pos()); }

  operator char(void) const;
  inline bool operator ==(const char& other) const { return operator char() == other; }
  inline bool operator !=(const char& other) const { return operator char() != other; }

  inline const char* dataAt(int position) { return m_data.data() + position; }
  inline HUString& data(void) { return m_data; }

  inline CCharStream& operator ++(int) { return ++*this; }
  inline CCharStream& operator --(int) { return --*this; }

  inline const CCharStream& next(void) const { ++m_reative_peek; return *this; }
  inline const CCharStream& prev(void) const { --m_reative_peek; return *this; }

  inline void seek(int pos) { m_reative_peek = 0; m_position = m_begin + pos; }

  inline int  size   (void) const { return m_size; }
  inline bool atEnd  (void) const { return m_position >= m_end; }
  inline int  fromEnd(void) const { return m_end - m_position; }
  inline int  pos    (void) const { return m_position - m_begin; }

  inline bool isSpace          (void) const { return QChar(operator char()).isSpace         (); }
  inline bool isNumber         (void) const { return QChar(operator char()).isNumber        (); }
  inline bool isLetter         (void) const { return QChar(operator char()).isLetter        (); }
  inline bool isLetterOrNumber (void) const { return QChar(operator char()).isLetterOrNumber(); }

private:
  char* m_position;
  char* m_begin;
  char* m_end;
  int    m_size;
  HUString m_data;
  mutable int m_reative_peek;
#ifdef DEBUG_CHAR_STREAM
  void*& m_position_void;
  void*& m_begin_void;
  void*& m_end_void;
#endif
};

#endif // CHAR_STREAM_H
