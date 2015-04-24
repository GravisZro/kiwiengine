#include "hustring.h"

#ifdef USE_UNICODE_LIB
HUString::HUString(void)
{
}

HUString::HUString(const QString& other)
: icu::UnicodeString(other.data_ptr(), other.size())
{

}

HUString::HUString(const icu::UnicodeString& other)
  : icu::UnicodeString(other)
{
}
#endif
