/*
 * le_disasm - Linear Executable disassembler
 */
/** @file regions.hpp
 *     Header file for regions.cpp, with declaration of Region class.
 * @par Purpose:
 *     Storage for Region class whose instances define what kind of
 *     content is stored in a specific range of addresses.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
#ifndef LEDISASM_REGIONS_H
#define LEDISASM_REGIONS_H

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

#endif // LEDISASM_REGIONS_H
