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

#include <inttypes.h>
#include <iostream>
#include <map>

class Analyser;

class Region
{
protected:
  friend class Analyser;

public:
  enum Type
  {
    UNKNOWN,
    CODE,
    DATA,
    VTABLE
  };

protected:
  uint32_t address;
  uint32_t size;
  Region::Type type;

public:
  Region (uint32_t address, uint32_t size = 1, Region::Type type = UNKNOWN);
  Region (void);
  Region (const Region &other);

  uint32_t get_address (void) const;
  size_t   get_end_address (void) const;
  Region::Type get_type (void) const;
  bool     contains_address (uint32_t addr) const;
  size_t   get_size (void) const;
};

std::ostream &operator<< (std::ostream &os, Region::Type type);
std::ostream &operator<< (std::ostream &os, const Region &reg);
