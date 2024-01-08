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
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdarg>
#include <sstream>
#include <stdexcept>

#include "disassembler.h"
#include "instruction.h"
#include "util.h"

static std::string
strip (std::string str)
{
  std::string::size_type n;

  n = str.find_first_not_of (" \t\r\n");
  if (n == std::string::npos)
    return "";

  str = str.substr (n);
  n = str.find_last_not_of (" \t\r\n");
  if (n == std::string::npos)
    return str;

  return str.substr (0, n + 1);
}

static std::string
lower (const std::string &str)
{
  std::string out;

  out.reserve (str.length ());
  std::transform (str.begin (), str.end (), std::back_inserter (out),
		  tolower);

  return out;
}


struct DisassemblerContext
{
  std::ostringstream string;
};


Disassembler::Disassembler (void)
{
  this->info = new disassemble_info;

  init_disassemble_info (this->info, NULL,
#ifdef HAVE_LIBOPCODES_DISASSEMBLER_STYLE
    &Disassembler::receive_instruction_text,
    &Disassembler::receive_instruction_styled_text);
#else
    &Disassembler::receive_instruction_text);
#endif

  this->info->arch               = bfd_arch_i386;
  this->info->mach               = bfd_mach_i386_i386;
  //this->info->disassembler_options = "intel-mnemonic"; // for intel syntax
  this->info->print_address_func = &Disassembler::print_address;
  //disassemble_init_for_target(this->info); // is this really needed?
  this->print_insn = disassembler(this->info->arch, false, this->info->mach, NULL);
}

Disassembler::Disassembler (const Disassembler &other)
{
  *this = other;
}

Disassembler &
Disassembler::operator= (const Disassembler &other)
{
  this->info = new disassemble_info (*other.info);
  return *this;
}

Disassembler::~Disassembler (void)
{
  delete this->info;
}

Instruction
Disassembler::disassemble (uint32_t addr, const std::string &data)
{
  Instruction inst;

  this->disassemble (addr, data.data (), data.length (), &inst);
  return inst;
}

void
Disassembler::disassemble (uint32_t addr, const void *data, size_t length,
			   Instruction *inst)
{
  DisassemblerContext context;
  int size;
  uint8_t data0, data1 = 0;
  bool have_target;

  assert (length > 0);

  this->info->buffer        = (bfd_byte *) data;
  this->info->buffer_length = length;
  this->info->buffer_vma    = addr;
  this->info->stream        = &context;

  size = this->print_insn (addr, this->info);
  if (size < 0)
    throw std::runtime_error ("Failed to disassemble instruction");

  inst->string = lower (strip (context.string.str ()));
  inst->size   = size;
  inst->type   = Instruction::MISC;
  inst->target = 0;

  if (size == 0)
    return;

  have_target = true;
  data0 = ((uint8_t *) data)[0];

  if (data0 == 0x2e)
    {
      if (size > 1)
	data0 = ((uint8_t *) data)[1];

      if (size > 2)
	data1 = ((uint8_t *) data)[2];
    }
  else
    {
      if (size > 1)
	data1 = ((uint8_t *) data)[1];
    }

    if (data0 == 0x0f)
      {
	if (data1 >= 0x80 and data1 < 0x90) /* j.. near */
	  inst->type = Instruction::COND_JUMP;
      }
    else if (data0 == 0xe8) /* call */
      inst->type = Instruction::CALL;
    else if (data0 == 0xe9) /* jmp near */
      inst->type = Instruction::JUMP;
    else if (data0 == 0x67 and data1 == 0xe3) /* 0x67 jmp short */
      inst->type = Instruction::JUMP;
    else if (data0 == 0xc2) /* retn */
      inst->type = Instruction::RET;
    else if (data0 == 0xca) /* lretn */
      inst->type = Instruction::RET;
    else if (data0 == 0xeb) /* jmp short */
      inst->type = Instruction::JUMP;
    else if (data0 >= 0x70 and data0 < 0x80) /* j.. short */
      inst->type = Instruction::COND_JUMP;
    else if (data0 >= 0xe0 and data0 <= 0xe3) /* loop */
      inst->type = Instruction::COND_JUMP;
    else if (data0 == 0xe3) /* jmp short */
      inst->type = Instruction::JUMP;
    else if (data0 == 0xcf) /* iret */
      inst->type = Instruction::RET;
    else if (data0 == 0xc3) /* ret */
      inst->type = Instruction::RET;
    else if (data0 == 0xcb) /* lret */
      inst->type = Instruction::RET;
    else if (data0 == 0xff) /* jmp near or call near indirect*/
      {
	have_target = false;

	/* whatever... */
	if (inst->string.find ("jmp") != std::string::npos)
	  inst->type = Instruction::JUMP;
	else
	  inst->type = Instruction::CALL;
      }

  if (have_target
      and (inst->type == Instruction::COND_JUMP
	   or inst->type == Instruction::JUMP
	   or inst->type == Instruction::CALL))
    {
      if (size < 5)
	inst->target =
	  addr + size
	  + read_s8 ((uint8_t *) data + size - sizeof (int8_t));
      else
	inst->target =
	  addr + size
	  + read_le<int32_t> ((uint8_t *) data + size - sizeof (int32_t));
    }
}

int
Disassembler::receive_instruction_text (void *context, const char *fmt, ...)
{
  va_list list;
  DisassemblerContext *ctx;
  char buffer[128];
  int ret;

  ctx = (DisassemblerContext *) context;

  va_start (list, fmt);
  ret = vsnprintf (buffer, sizeof (buffer) - 1, fmt, list);
  buffer[ret] = 0;
  va_end (list);

  ctx->string << buffer;

  return ret;
}

#ifdef HAVE_LIBOPCODES_DISASSEMBLER_STYLE
int
Disassembler::receive_instruction_styled_text (void *context,
		enum disassembler_style style, const char *fmt, ...)
{
  va_list list;
  DisassemblerContext *ctx;
  char buffer[128];
  int ret;

  ctx = (DisassemblerContext *) context;

  va_start (list, fmt);
  ret = vsnprintf (buffer, sizeof (buffer) - 1, fmt, list);
  buffer[ret] = 0;
  va_end (list);

  ctx->string << buffer;

  return ret;
}
#endif

void
Disassembler::print_address (bfd_vma address, disassemble_info *info)
{
  info->fprintf_func (info->stream, "0x%llx", (unsigned long long)address);
}
