/*
 * le_disasm - Linear Executable disassembler
 */
/** @file instruction.hpp
 *     Header file for instruction.cpp, with declaration of Instruction class.
 * @par Purpose:
 *     Storage for Instruction class which represents a single assembly
 *     instruction, and stores its properties.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
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
