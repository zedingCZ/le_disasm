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

#include "regions.h"
#include "util.h"

using std::ios;

Region::Region (uint32_t address, uint32_t size, Region::Type type)
{
  this->address = address;
  this->size    = size;
  this->type    = type;
}

Region::Region (void)
{
  this->address = 0;
  this->size    = 0;
  this->type    = UNKNOWN;
}

Region::Region (const Region &other)
{
  *this = other;
}

uint32_t
Region::get_address (void) const
{
  return this->address;
}

size_t
Region::get_end_address (void) const
{
  return (this->address + this->size);
}

Region::Type
Region::get_type (void) const
{
  return this->type;
}

bool
Region::contains_address (uint32_t addr) const
{
  return (this->address <= addr and addr < this->address + this->size);
}

size_t
Region::get_size (void) const
{
  return this->size;
}

std::ostream &
operator<< (std::ostream &os, Region::Type type)
{
  switch (type)
    {
    case Region::UNKNOWN: os << "unknown";   break;
    case Region::CODE:    os << "code";      break;
    case Region::DATA:    os << "data";      break;
    case Region::VTABLE:  os << "vtable";    break;
    default:              os << "(unknown)"; break;
    }
  return os;
}

std::ostream &
operator<< (std::ostream &os, const Region &reg)
{
  PUSH_IOS_FLAGS (&os);

  os.setf (ios::hex, ios::basefield);
  os.setf (ios::showbase);

  os << reg.get_type () << " at " << reg.get_address ()
     << ", size " << reg.get_size ();

  return os;
}
