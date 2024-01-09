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
#ifndef SWDISASM_IMAGE_H
#define SWDISASM_IMAGE_H

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

#endif
