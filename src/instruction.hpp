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
#ifndef LEDISASM_INSTRUCTION_H
#define LEDISASM_INSTRUCTION_H

#include <inttypes.h>
#include <string>

struct Instruction
{
protected:
  friend class Disassembler;

public:
  enum Type
  {
    MISC,
    COND_JUMP,
    JUMP,
    CALL,
    RET
  };

protected:
  Type        type;
  std::string string;
  uint32_t    target;
  size_t      size;

public:
  Type        get_type (void);
  std::string get_string (void);
  uint32_t    get_target (void);
  size_t      get_size (void);
};

#endif // LEDISASM_INSTRUCTION_H
