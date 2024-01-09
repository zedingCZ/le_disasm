/* 
 * swdisasm - LE disassembler for Syndicate Wars
 * 
 * Copyright (C) 2010  Unavowed <unavowed@vexillium.org>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LEDISASM_UTIL_H
#define LEDISASM_UTIL_H

#include <ostream>
#include <map>
#include <set>
#include <stdint.h>

#ifdef LITTLE_ENDIAN
# undef LITTLE_ENDIAN
#endif

#ifdef BIG_ENDIAN
# undef BIG_ENDIAN
#endif

enum Endianness
{
  LITTLE_ENDIAN,
  BIG_ENDIAN
};

std::ostream &operator<< (std::ostream &os, const Endianness &end);


class IOSFormatSaver
{
protected:
  std::ios::fmtflags flags;
  std::ios *stream;

public:
  IOSFormatSaver (std::ios *str)
  {
    this->stream = str;
    this->flags = str->flags ();
  }

  ~IOSFormatSaver (void)
  {
    this->stream->flags (this->flags);
  }
};

#define PUSH_IOS_FLAGS(sptr) \
  IOSFormatSaver xxx_asdf_fmt_saver (sptr);


template <typename K, typename V, typename C, typename A>
V *
get_next_value (std::map<K, V, C, A> *map, const K &key)
{
  typename std::map<K, V, C, A>::iterator itr;

  itr = map->upper_bound (key);
  if (itr == map->end ())
    return NULL;

  return &itr->second;
}

template <typename K, typename C, typename A>
const K *
get_next_value (const std::set<K, C, A> *set, const K &key)
{
  typename std::set<K, C, A>::iterator itr;

  itr = set->upper_bound (key);
  if (itr == set->end ())
    return NULL;

  return &*itr;
}

template <typename MapType>
typename MapType::value_type::second_type *
get_previous_value (MapType *map, const typename MapType::key_type &key)
{
  typename MapType::iterator itr;

  itr = map->lower_bound (key);

  if (itr == map->begin ())
    return NULL;

  --itr;
  return &itr->second;
}


template <typename T, size_t Bytes>
void
write_le (const void *memory, T value)
{
  size_t n;
  uint8_t *p;

  p = (uint8_t *) memory;

  for (n = 0; n < Bytes; n++)
    {
      p[n] = value & 0xff;
      value >>= 8;
    }
}

template <typename T>
void
write_le (const void *memory, T value)
{
  write_le<T, sizeof (T)> (memory, value);
}

template <typename T, size_t Bytes>
T
read_le (const void *memory)
{
  T value = T (0);
  uint8_t *p;
  size_t n;

  p = (uint8_t *) memory;

  for (n = 0; n < Bytes; n++)
    value |= (T) p[n] << (n * 8);

  return value;
}

template <typename T>
T
read_le (const void *memory)
{
  return read_le<T, sizeof (T)> (memory);
}

static inline int8_t
read_s8 (const void *memory)
{
  return *(int8_t *) memory;
}

template <typename T>
void
print_variable (std::ostream *os, size_t value_column,
		const std::string &name, const T &value, size_t indent = 2)
{
  size_t pos;
  std::ios::fmtflags flags;

  flags = os->flags ();
  os->setf (std::ios::hex, std::ios::basefield);
  os->setf (std::ios::showbase);

  for (pos = 0; pos < indent; pos++)
    *os << ' ';

  pos += name.length () + 2;

  *os << name << ": ";
  
  while (pos + 1 < value_column)
    {
      *os << '.';
      pos++;
    }

  *os << ' ' << value << '\n';

  os->flags (flags);
}

#endif // LEDISASM_UTIL_H
