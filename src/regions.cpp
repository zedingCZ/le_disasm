/*
 * le_disasm - Linear Executable disassembler
 */
/** @file regions.cpp
 *     Implementation of Region class methods.
 * @par Purpose:
 *     Implements the Region class whose instances define what kind of
 *     content is stored in a specific range of addresses.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
#include "regions.hpp"
#include "util.hpp"

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
