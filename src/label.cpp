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

#include <iostream>

#include "label.hpp"
#include "util.hpp"

Label::Label (uint32_t address, Label::Type type,
  const std::string &name)
{
  this->address = address;
  this->name    = name;
  this->type    = type;
}

Label::Label (void)
{
  this->address = 0;
  this->name    = "";
  this->type    = UNKNOWN;
}

Label::Label (const Label &other)
{
  *this = other;
}

uint32_t
Label::get_address (void) const
{
  return this->address;
}

Label::Type
Label::get_type (void) const
{
  return this->type;
}

std::string
Label::get_name (void) const
{
  return this->name;
}

std::ostream &
operator<< (std::ostream &os, const Label &label)
{
  PUSH_IOS_FLAGS (&os);

  if (label.get_name ().empty ())
    {
      std::string prefix;

      switch (label.get_type ())
        {
        case Label::FUNCTION: prefix = "func";    break;
        case Label::JUMP:     prefix = "jump";    break;
        case Label::DATA:     prefix = "data";    break;
        case Label::VTABLE:   prefix = "vtable";  break;
        default:              prefix = "unknown"; break;
        }

      os << prefix << "_" << std::hex << std::noshowbase
         << label.get_address ();
    }
  else
    os << label.get_name ();

  return os;
}
