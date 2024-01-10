/*
 * le_disasm - Linear Executable disassembler
 */
/** @file image.cpp
 *     Implementation of Image and Object class methods.
 * @par Purpose:
 *     Implements the Image class methods for handling a list of Objects
 *     received from LE file content.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
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
