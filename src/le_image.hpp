/*
 * le_disasm - Linear Executable disassembler
 */
/** @file le_image.hpp
 *     Header file for le_image.cpp, wrapper for Image instance creation.
 * @par Purpose:
 *     Declares functions neccessary for Image creation based on
 *     LinearExecutable headers.
 * @author   Unavowed <unavowed@vexillium.org>
 * @date     2010-09-20 - 2024-01-10
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
#ifndef LEDISASM_LE_IMAGE_H
#define LEDISASM_LE_IMAGE_H

#include <istream>

class Image;
class LinearExecutable;

Image *create_image (std::istream *is, const LinearExecutable *lx);

#endif // LEDISASM_LE_IMAGE_H
