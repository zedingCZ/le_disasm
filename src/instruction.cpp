/*
 * le_disasm - Linear Executable disassembler
 */
/** @file instruction.cpp
 *     Implementation of Instruction class methods.
 * @par Purpose:
 *     Implementation of Instruction class which represents a single assembly
 *     instruction, and stores its properties.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
#include "instruction.hpp"

Instruction::Type
Instruction::get_type (void)
{
  return this->type;
}

std::string
Instruction::get_string (void)
{
  return this->string;
}

uint32_t
Instruction::get_target (void)
{
  return this->target;
}

size_t
Instruction::get_size (void)
{
  return this->size;
}
