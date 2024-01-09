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

#ifndef LEDISASM_LABEL_H
#define LEDISASM_LABEL_H

#include <inttypes.h>
#include <string>

class LinearExecutable;
class Image;
class Region;

class Label
{
public:
  enum Type
  {
    UNKNOWN,
    JUMP,
    FUNCTION,
    VTABLE,
    DATA
  };

protected:
  uint32_t address;
  std::string name;
  Label::Type type;

public:
  Label (uint32_t address, Label::Type type = UNKNOWN,
         const std::string &name = "");
  Label (void);
  Label (const Label &other);

  uint32_t  get_address (void) const;
  Label::Type  get_type (void) const;
  std::string  get_name (void) const;
};

std::ostream &operator<< (std::ostream &os, const Label &label);

#endif // LEDISASM_LABEL_H
