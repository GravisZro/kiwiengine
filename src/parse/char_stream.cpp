#include "char_stream.h"

#include <soloutils>


CCharStream::CCharStream(void)
  : m_position(nullptr),
    m_begin(nullptr),
    m_end(nullptr),
    m_reative_peek(0)
#ifdef DEBUG_CHAR_STREAM
    , m_position_void(reinterpret_cast<void*&>(m_position)),
    m_begin_void(reinterpret_cast<void*&>(m_begin)),
    m_end_void(reinterpret_cast<void*&>(m_end))
#endif
{
}

void CCharStream::setData(const HUString& data)
{
  m_data = data;
  m_size = m_data.size();
  m_position = m_begin = m_data.data();
  m_end = m_begin + m_size;
}

CCharStream::operator char(void) const
{
  if(m_reative_peek)
  {
    register char* peek_pos = m_position + m_reative_peek;
    m_reative_peek = 0;

    if(peek_pos < m_begin)
      return '\n';
    if(peek_pos >= m_end)
      return 0;
    return *peek_pos;
  }
  else
    return *m_position;
}

bool CCharStream::operator ==(const HUString& other) const
{
  if(fromEnd() < other.size())
    return false;
  return !qstrnicmp(m_position, other.data(), other.size());
}

CCharStream& CCharStream::operator +=(int length)
{
  if(m_position + length > m_end)
    throw(complaint("end of stream"));
  m_position += length;
  return *this;
}

CCharStream& CCharStream::operator ++(void)
{
  if(m_position > m_end)
    throw(complaint("end of stream"));
  ++m_position;
  return *this;
}

CCharStream& CCharStream::operator --(void)
{
  if(m_position == m_begin)
    throw(complaint("start of stream"));
  --m_position;
  return *this;
}

int CCharStream::indexInLine(const char& target) const
{
  char* pos = m_position;
  while(pos < m_end &&
        *pos != target &&
        *pos != ECharacter::endline)
    ++pos;
  return pos - m_begin;
}
