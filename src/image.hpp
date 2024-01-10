/*
 * le_disasm - Linear Executable disassembler
 */
/** @file image.hpp
 *     Header file for image.cpp, with declaration of Image and Object classes.
 * @par Purpose:
 *     Storage for Image class which handles a list of Objects received
 *     from LE file content.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
#ifndef LEDISASM_IMAGE_H
#define LEDISASM_IMAGE_H

#include <inttypes.h>
#include <cstddef>
#include <vector>

class Image
{
public:
  typedef std::vector<uint8_t> DataVector;

public:
  class Object
  {
  protected:
    friend class Image;

  protected:
    size_t index;
    uint32_t base_address;
    bool executable;
    DataVector data;

  public:
    Object (size_t index, uint32_t base_address, bool executable,
	    const DataVector *data = NULL);
    Object (const Object &other);
    size_t get_index (void) const;
    const DataVector *get_data (void) const;
    const uint8_t *get_data_at (uint32_t address) const;
    uint32_t get_base_address (void) const;
    bool is_executable (void) const;
  };

protected:
  std::vector<Object> objects;

public:
  Image (const std::vector<Object> *objects);
  const Object *get_object (size_t index) const;
  const Object *get_object_at_address (uint32_t address) const;
  size_t get_object_count (void) const;
};

#endif // LEDISASM_IMAGE_H
