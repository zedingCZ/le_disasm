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
#include <cstddef>

#include "image.hpp"

Image::Object::Object (size_t index, uint32_t base_address, bool executable,
		       const DataVector *data)
{
  if (data != NULL)
    this->data = *data;

  this->index        = index;
  this->base_address = base_address;
  this->executable   = executable;
}

Image::Object::Object (const Object &other)
{
  this->data         = other.data;
  this->index        = other.index;
  this->base_address = other.base_address;
  this->executable   = other.executable;
}

const Image::DataVector *
Image::Object::get_data (void) const
{
  return &this->data;
}

const uint8_t *
Image::Object::get_data_at (uint32_t address) const
{
  return (&this->data.front () + address - this->get_base_address ());
}

size_t
Image::Object::get_index (void) const
{
  return this->index;
}

uint32_t
Image::Object::get_base_address (void) const
{
  return this->base_address;
}

bool
Image::Object::is_executable (void) const
{
  return this->executable;
}


Image::Image (const std::vector<Object> *objects)
{
  this->objects = *objects;
}

const Image::Object *
Image::get_object (size_t index) const
{
  if (index >= this->objects.size ())
    return NULL;

  return &this->objects[index];
}

size_t
Image::get_object_count (void) const
{
  return this->objects.size ();
}

const Image::Object *
Image::get_object_at_address (uint32_t address) const
{
  const Image::Object *obj;
  size_t n;

  for (n = 0; n < this->objects.size (); n++)
    {
      obj = &this->objects[n];

      if (obj->base_address <= address
	  and address < obj->base_address + obj->data.size ())
	return obj;
    }

  return NULL;
}
