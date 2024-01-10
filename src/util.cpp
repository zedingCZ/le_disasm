/*
 * le_disasm - Linear Executable disassembler
 */
/** @file util.cpp
 *     Implementation of few small but useful utilities.
 * @par Purpose:
 *     Implements utility functions to perform often required tasks.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
#include "util.hpp"

std::ostream &
operator<< (std::ostream &os, const Endianness &end)
{
  if (end == LITTLE_ENDIAN)
    os << "LITTLE_ENDIAN";
  else
    os << "BIG_ENDIAN";

  return os;
}
