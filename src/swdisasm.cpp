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
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include "disassembler.h"
#include "instruction.h"
#include "le.h"
#include "le_image.h"
#include "regions.h"
#include "util.h"

using std::ios;


typedef Image::DataVector IDV;
typedef Image::Object Object;
typedef LinearExecutable::FixupMap LEFM;
typedef LinearExecutable::Header LEH;
typedef LinearExecutable::ObjectHeader LEOH;


class Analyser;


class Label
{
public:
  enum Type
  {
    UNKNOWN,
    JUMP,
    FUNCTION,
    VTABLE,
    DATA
  };

protected:
  uint32_t address;
  std::string name;
  Type type;

public:
  Label (uint32_t address, Type type = UNKNOWN,
	 const std::string &name = "")
  {
    this->address = address;
    this->name    = name;
    this->type    = type;
  }

  Label (void)
  {
    this->address = 0;
    this->name    = "";
    this->type    = UNKNOWN;
  }

  Label (const Label &other)
  {
    *this = other;
  }

  uint32_t
  get_address (void) const
  {
    return this->address;
  }

  Type
  get_type (void) const
  {
    return this->type;
  }

  std::string
  get_name (void) const
  {
    return this->name;
  }
};

std::ostream &
operator<< (std::ostream &os, const Label &label)
{
  PUSH_IOS_FLAGS (&os);

  if (label.get_name ().empty ())
    {
      std::string prefix;

      switch (label.get_type ())
	{
	case Label::FUNCTION: prefix = "func";    break;
	case Label::JUMP:     prefix = "jump";    break;
	case Label::DATA:     prefix = "data";    break;
	case Label::VTABLE:   prefix = "vtable";  break;
	default:              prefix = "unknown"; break;
	}

      os << prefix << "_" << std::hex << std::noshowbase
	 << label.get_address ();
    }
  else
    os << label.get_name ();

  return os;
}


class Analyser
{
public:
  typedef std::map<uint32_t, Region> RegionMap;
  typedef std::map<uint32_t, Label>  LabelMap;

protected:
  RegionMap            regions;
  LabelMap             labels;
  std::deque<uint32_t> code_trace_queue;
  LinearExecutable    *le;
  Image               *image;
  Disassembler         disasm;

protected:
  void
  add_region (const Region &reg)
  {
    this->regions[reg.get_address ()] = reg;
  }

  void
  add_initial_regions (void)
  {
    const LEOH *ohdr;
    size_t n;
    Region::Type type;

    for (n = 0; n < this->le->get_object_count (); n++)
      {
	ohdr = this->le->get_object_header (n);

	if ((ohdr->flags & LEOH::EXECUTABLE) == 0)
	  {
	    type = Region::DATA;
	    this->set_label (Label (ohdr->base_address, Label::DATA));
	  }
	else
	  type = Region::UNKNOWN;

	this->add_region (Region (ohdr->base_address,
				  ohdr->virtual_size, type));
      }
  }

  void
  add_eip_to_trace_queue (void)
  {
    const LEOH *ohdr;
    const LEH *hdr;
    uint32_t eip;

    hdr = this->le->get_header ();
    ohdr = this->le->get_object_header (hdr->eip_object_index);
    eip = ohdr->base_address + hdr->eip_offset;
    this->add_code_trace_address (eip);
    this->set_label (Label (eip, Label::FUNCTION, "_start"));
  }

  void
  add_code_trace_address (uint32_t addr)
  {
    this->code_trace_queue.push_back (addr);
  }

  void
  trace_code (void)
  {
    uint32_t address;

    while (!this->code_trace_queue.empty ())
      {
	address = this->code_trace_queue.front ();
	this->code_trace_queue.pop_front ();
	this->trace_code_at_address (address);
      }
  }

  void
  trace_code_at_address (uint32_t start_addr)
  {
    Region *reg;
    size_t end_addr;
    size_t addr;
    const Object *obj;
    const IDV *data;
    Instruction inst;
    const void *data_ptr;

    reg = this->get_region_at_address (start_addr);
    if (reg == NULL)
      {
	std::cerr << "Warning: Tried to trace code at an unmapped address: "
	          << start_addr << ".\n";
	return;
      }

    if (reg->get_type () == Region::CODE) /* already traced */
      return;

    end_addr = reg->get_end_address ();
    obj = this->image->get_object_at_address (start_addr);
    data = obj->get_data ();

    addr = start_addr;

    while (addr < end_addr)
      {
	data_ptr = &data->front () + addr - obj->get_base_address ();
	this->disasm.disassemble (addr, data_ptr, end_addr - addr, &inst);

	if (inst.get_target () != 0)
	  {
	    switch (inst.get_type ())
	      {
	      case Instruction::CALL:
		this->set_label (Label (inst.get_target (), Label::FUNCTION));
		this->add_code_trace_address (inst.get_target ());
		break;

	      case Instruction::COND_JUMP:
	      case Instruction::JUMP:
		this->set_label (Label (inst.get_target (), Label::JUMP));
		this->add_code_trace_address (inst.get_target ());
		break;

	      default:
		break;
	      }
	  }

	addr += inst.get_size();

	switch (inst.get_type ())
	  {
	  case Instruction::JUMP:
	  case Instruction::RET:
	    goto end;

	  default:
	    break;
	  }
      }

  end:
    this->insert_region
      (reg, Region (start_addr, addr - start_addr, Region::CODE));
  }

  Region *
  get_previous_region (const Region *reg)
  {
    return get_previous_value (&this->regions, reg->get_address ());
  }

  Region *
  get_region_at_address (uint32_t address)
  {
    RegionMap::iterator itr;

    if (this->regions.empty ())
      return NULL;

    itr = this->regions.lower_bound (address);

    if (itr == this->regions.end ())
      {
	--itr;
	
	if (itr->second.contains_address (address))
	  return &itr->second;
	else
	  return NULL;
      }

    if (itr->first == address)
      return &itr->second;

    if (itr == this->regions.begin ())
      return NULL;

    --itr;

    if (!itr->second.contains_address (address))
      return NULL;

    return &itr->second;
  }

  Region *
  get_region (uint32_t address)
  {
    RegionMap::iterator itr;

    itr = this->regions.find (address);
    if (itr == this->regions.end ())
      return NULL;

    return &itr->second;
  }

  void
  insert_region (Region *parent, const Region &reg)
  {
    assert (parent->contains_address (reg.get_address ()));
    assert (parent->contains_address (reg.get_end_address () - 1));

    if (reg.get_end_address () != parent->get_end_address ())
      {
	this->add_region
	  (Region (reg.get_end_address (),
		   parent->get_end_address () - reg.get_end_address (),
		   parent->get_type ()));
      }

    if (reg.get_address () != parent->get_address ())
      {
	this->add_region (reg);

	parent->size = reg.get_address () - parent->get_address ();
      }
    else
      *parent = reg;

    this->check_merge_regions (reg.address);
  }

  void
  check_merge_regions (uint32_t addr)
  {
    Region *reg;
    Region *prev;
    Region *next;

    reg = this->get_region (addr);
    prev = this->get_previous_region (reg);
    next = this->get_next_region (reg);

    if (prev != NULL and prev->get_type () == reg->get_type ()
	and prev->get_end_address () == reg->get_address ())
      {
	prev->size += reg->size;
	this->regions.erase (addr);
	reg = prev;
      }

    if (next != NULL and reg->get_type () == next->get_type ()
	and reg->get_end_address () == next->get_address ())
      {
	reg->size += next->size;
	this->regions.erase (next->get_address ());
      }
  }

  void
  trace_vtables (void)
  {
    const LEFM *fixups;
    LEFM::const_iterator itr;
    const Image::Object *obj;
    const uint8_t *data_ptr;
    Region *reg;
    size_t off;
    size_t size;
    size_t count;
    size_t n;
    uint32_t addr;
    const uint32_t *aptr;

    PUSH_IOS_FLAGS (&std::cerr);
    std::cerr.setf (ios::hex, ios::basefield);
    std::cerr.setf (ios::showbase);

    for (n = 0; n < this->le->get_object_count (); n++)
      {
	fixups = this->le->get_fixups_for_object (n);

	for (itr = fixups->begin (); itr != fixups->end (); ++itr)
	  {
	    reg = this->get_region_at_address (itr->second.address);
	    if (reg == NULL)
	      {
		std::cerr << "Warning: Reloc pointing to unmapped memory at "
		          << itr->second.address << ".\n";
		continue;
	      }

	    if (reg->get_type () != Region::UNKNOWN)
	      continue;

	    obj = this->image->get_object_at_address (reg->get_address ());
	    if (!obj->is_executable ())
	      continue;
	    size = reg->get_end_address () - itr->second.address;
	    aptr = get_next_value (this->le->get_fixup_addresses (),
				   itr->second.address);
	    if (aptr != NULL)
	      size = std::min<size_t> (size, *aptr - itr->second.address);


	    data_ptr = obj->get_data_at (itr->second.address);
	    count = 0;
	    off = 0;

	    while (off + 4 <= size)
	      {
		addr = read_le<uint32_t> (data_ptr + off);

		if (addr == 0
		    or fixups->find (itr->second.address + off
				     - obj->get_base_address ())
		       != fixups->end ())
		  {
		    count++;

		    if (addr != 0)
		      {
			this->set_label (Label (addr, Label::FUNCTION));
			this->add_code_trace_address (addr);
		      }
		  }
		else
		  break;

		off += 4;
	      }

	    if (count > 0)
	      {
		this->insert_region (reg, Region (itr->second.address,
						  4 * count, Region::VTABLE));
		this->set_label (Label (itr->second.address, Label::VTABLE));
		this->trace_code ();
	      }
	  }
      }
  }

  void
  trace_remaining_relocs (void)
  {
    const LEFM *fixups;
    LEFM::const_iterator itr;
    Region *reg;
    size_t n;
    size_t guess_count = 0;
    const Label *label;

    PUSH_IOS_FLAGS (&std::cerr);
    std::cerr.setf (ios::hex, ios::basefield);
    std::cerr.setf (ios::showbase);

    for (n = 0; n < this->image->get_object_count (); n++)
      {
	fixups = this->le->get_fixups_for_object (n);

	for (itr = fixups->begin (); itr != fixups->end (); ++itr)
	  {
	    reg = this->get_region_at_address (itr->second.address);
	    if (reg == NULL
		or (reg->get_type () != Region::UNKNOWN
		    and reg->get_type () != Region::DATA))
	      continue;

	    if (reg->get_type () == Region::UNKNOWN)
	      {
		label = this->get_label (itr->second.address);

		if (label == NULL
		    or (label->get_type () != Label::FUNCTION
			and label->get_type () != Label::JUMP))
		  {
		    std::cerr << "Guessing that " << itr->second.address
			      << " is a function.\n";
		    guess_count++;
		    this->set_label (Label (itr->second.address,
					    Label::FUNCTION));
		  }

		this->add_code_trace_address (itr->second.address);
		this->trace_code ();
	      }
	    else
	      {
		this->set_label (Label (itr->second.address,
					Label::DATA));
	      }
	  }
      }

    std::cerr.setf (ios::dec, ios::basefield);
    std::cerr.unsetf (ios::showbase);
    std::cerr << guess_count << " guess(es) to investigate.\n";
  }

public:
  Analyser (void)
  {
    this->le    = NULL;
    this->image = NULL;
  }

  Analyser (const Analyser &other)
  {
    *this = other;
  }

  Analyser (LinearExecutable *le, Image *img)
  {
    this->le    = le;
    this->image = img;
    this->add_initial_regions ();
  }

  Analyser &
  operator= (const Analyser &other)
  {
    this->le     = other.le;
    this->image  = other.image;
    this->disasm = other.disasm;
    this->add_initial_regions ();
    return *this;
  }

  Label *
  get_next_label (const Label *lab)
  {
    return get_next_value (&this->labels, lab->get_address ());
  }

  Label *
  get_next_label (uint32_t addr)
  {
    return get_next_value (&this->labels, addr);
  }

  Region *
  get_next_region (const Region *reg)
  {
    return get_next_value (&this->regions, reg->get_address ());
  }

  void
  insert_region (const Region &reg)
  {
    Region *parent;
    parent = this->get_region_at_address (reg.get_address ());
    this->insert_region (parent, reg);
  }

  void
  set_label (const Label &lab)
  {
    const Label *label;

    label = this->get_label (lab.get_address ());
    if (label != NULL)
      {
	if (label->get_type () == Label::FUNCTION
	    or !label->get_name ().empty ())
	  return;
      }

    this->labels[lab.get_address ()] = lab;
  }

  void
  remove_label (uint32_t addr)
  {
    this->labels.erase (addr);
  }

  void
  run (void)
  {
    this->add_eip_to_trace_queue ();
    std::cerr << "Tracing code directly accessible from the entry point...\n";
    this->trace_code ();
    std::cerr << "Tracing text relocs for vtables...\n";
    this->trace_vtables ();
    std::cerr << "Tracing remaining relocs for functions and data...\n";
    this->trace_remaining_relocs ();
  }

  const RegionMap *
  get_regions (void) const
  {
    return &this->regions;
  }

  const LabelMap *
  get_labels (void) const
  {
    return &this->labels;
  }

  const Label *
  get_label (uint32_t addr) const
  {
    LabelMap::const_iterator itr;

    itr = this->labels.find (addr);
    if (itr == this->labels.end ())
      return NULL;

    return &itr->second;
  }
};


static void
print_separator (void)
{
  int n;

  std::cout << "/*";

  for (n = 0; n < 64; n++)
    std::cout << '-';

  std::cout << "*/\n";
}

static void
print_label (const Label *lab)
{
  int indent;

  switch (lab->get_type ())
    {
    case Label::FUNCTION:
      std::cout << "\n\n";
      print_separator ();
      indent = 0;
      break;

    case Label::JUMP:
      indent = 1;
      break;

    case Label::DATA:
      indent = 0;
      break;

    case Label::VTABLE:
      indent = 0;
      std::cout << '\n';
      break;

    default:
      indent = 0;
      break;
    }

  while (indent-- > 0)
    std::cout << '\t';

  std::cout << *lab << ":";

  if (!lab->get_name ().empty ())
    {
      PUSH_IOS_FLAGS (&std::cout);
      std::cout.setf (ios::hex, ios::basefield);
      std::cout.setf (ios::showbase);

      std::cout << "\t/* " << lab->get_address () << " */";
    }

  std::cout << '\n';

  switch (lab->get_type ())
    {
    case Label::FUNCTION:
      print_separator ();
      break;

    default:
      break;
    }
}

static std::string
replace_addresses_with_labels (const std::string &str, Image *img,
			       LinearExecutable *le, Analyser *anal)
{
  std::ostringstream oss;
  const Label *lab;
  size_t n, start;
  uint32_t addr;
  std::string addr_str;
  std::string comment;

  n = str.find ("0x");
  if (n == std::string::npos)
    return str;

  start = 0;

  do
    {
      oss << str.substr (start, n - start);

      if (n + 2 >= str.length ())
	break;

      n += 2;
      start = n;

      while (n < str.length () and isxdigit (str[n]))
	n++;

      addr_str = str.substr (start, n - start);
      addr = strtol (addr_str.c_str (), NULL, 16);
      lab = anal->get_label (addr);
      if (lab != NULL)
	oss << *lab;
      else
	{
	  oss << "0x" << addr_str;

	  if (img->get_object_at_address (addr) != NULL
	      and le->get_fixup_addresses ()->find (addr)
		  != le->get_fixup_addresses ()->end ())
	    {
	      comment = " /* Warning: address points to a valid object/reloc, "
		        "but no label found */";
	    }
	}

      start = n;
      n = str.find ("0x", start);
    }
  while (n != std::string::npos);

  if (start < str.length ())
    oss << str.substr (start);

  if (!comment.empty ())
    oss << comment;

  return oss.str ();
}

static void
print_instruction (Instruction *inst, Image *img, LinearExecutable *le,
		   Analyser *anal)
{
  std::string str;
  std::string::size_type n;

  str = replace_addresses_with_labels (inst->get_string (), img, le, anal);

  n = str.find ("(287 only)");
  if (n != std::string::npos)
    {
      std::cout << "\t\t/* " << str << " -- ignored */\n";
      return;
    }

  /* Work around buggy libopcodes */
  if (str == "lar    %cx,%ecx")
    str = "lar    %ecx,%ecx";
  else if (str == "lsl    %ax,%eax")
    str = "lsl    %eax,%eax";

  std::cout << "\t\t" << str;

  if (str == "data16" or str == "data32")
    std::cout << " ";
  else
    std::cout << "\n";
}

static bool
data_is_address (const Object *obj, uint32_t addr, size_t len,
		 LinearExecutable *le)
{
  uint32_t offset;
  const LEFM *fups;

  if (len < 4)
    return false;

  fups = le->get_fixups_for_object (obj->get_index ());
  offset = addr - obj->get_base_address ();

  if (fups->find (offset) == fups->end ())
    return false;

  return true;
}

static bool
data_is_zeros (const Object *obj, uint32_t addr, size_t len, size_t *rlen)
{
  size_t x;
  const uint8_t *data;

  data = obj->get_data_at (addr);

  for (x = 0; x < len; x++)
    {
      if (data[x] != 0)
	break;
    }

  if (x < 4)
    return false;

  *rlen = x;
  return true;
}

static bool
data_is_string (const Object *obj, uint32_t addr, size_t len, size_t *rlen,
		bool *zero_terminated)
{
  size_t x;
  const uint8_t *data;

  data = obj->get_data_at (addr);

  for (x = 0; x < len; x++)
    {
      if ((data[x] < 0x20 or data[x] >= 0x7f)
	  and not (data[x] == '\t' or data[x] == '\n' or data[x] == '\r'))
	break;
    }

  if (x < 4)
    return false;

  if (x < len and data[x] == 0)
    {
      *zero_terminated = true;
      x += 1;
    }
  else
    *zero_terminated = false;

  *rlen = x;
  return true;
}

static void
print_escaped_string (const uint8_t *data, size_t len)
{
  size_t n;

  for (n = 0; n < len; n++)
    {
      if (data[n] == '\t')
	std::cout << "\\t";
      else if (data[n] == '\r')
	std::cout << "\\r";
      else if (data[n] == '\n')
	std::cout << "\\n";
      else if (data[n] == '\\')
	std::cout << "\\\\";
      else if (data[n] == '"')
	std::cout << "\\\"";
      else
	std::cout << (char) data[n];
    }
}

static void
print_region (const Region *reg, const Object *obj, LinearExecutable *le,
	      Image *img, Analyser *anal)
{
  const Label *label;
  size_t addr;
  int bytes_in_line;
  Disassembler disasm;
  Instruction inst;
  LEFM::const_iterator itr;

#ifdef ENABLE_DEBUG
  std::cout << "Region: " << *reg << ":\n";
#endif

  obj = img->get_object_at_address (reg->get_address ());
  addr = reg->get_address ();

  switch (reg->get_type ())
    {
    case Region::CODE:
      while (addr < reg->get_end_address ())
	{
	  label = anal->get_label (addr);
	  if (label != NULL)
	    print_label (label);

	  disasm.disassemble (addr, obj->get_data_at (addr),
			      reg->get_end_address () - addr, &inst);
	  print_instruction (&inst, img, le, anal);

	  addr += inst.get_size ();
	}
      break;

    case Region::DATA:
      size_t len;
      size_t size;
      const Label *label;
      const LEFM *fups;
      bool zt;

      bytes_in_line = 0;

      fups = le->get_fixups_for_object (obj->get_index ());
      itr = fups->begin ();

      while (addr < reg->get_end_address ())
	{
	  label = anal->get_label (addr);
	  if (label != NULL)
	    {
	      if (bytes_in_line > 0)
		{
		  std::cout << "\"\n";
		  bytes_in_line = 0;
		}

	      print_label (label);
	    }

	  len = reg->get_end_address () - addr;

	  label = anal->get_next_label (addr);
	  if (label != NULL)
	    len = std::min (len, label->get_address () - addr);

	  while (itr != fups->end ()
		 and itr->first <= addr - obj->get_base_address ())
	    ++itr;

	  if (itr != fups->end ())
	    len = std::min<size_t> (len,
				    itr->first
				    - (addr - obj->get_base_address ()));

	  while (len > 0)
	    {
	      if (data_is_address (obj, addr, len, le))
		{
		  const Label *dlabel;
		  uint32_t value;

		  if (bytes_in_line > 0)
		    {
		      std::cout << "\"\n";
		      bytes_in_line = 0;
		    }

		  value = read_le<uint32_t> (obj->get_data_at (addr));
		  dlabel = anal->get_label (value);
		  assert (dlabel != NULL);
		  std::cout << "\t\t.long   " << *dlabel << "\n";

		  addr += 4;
		  len -= 4;
		}
	      else if (data_is_zeros (obj, addr, len, &size))
		{
		  PUSH_IOS_FLAGS (&std::cout);
		  std::cout.setf (ios::hex, ios::basefield);
		  std::cout.setf (ios::showbase);

		  if (bytes_in_line > 0)
		    {
		      std::cout << "\"\n";
		      bytes_in_line = 0;
		    }

		  std::cout << "\t\t.fill   " << size << "\n";
		  addr += size;
		  len -= size;
		}
	      else if (data_is_string (obj, addr, len, &size, &zt))
		{
		  if (bytes_in_line > 0)
		    {
		      std::cout << "\"\n";
		      bytes_in_line = 0;
		    }

		  if (zt)
		    std::cout << "\t\t.string \"";
		  else
		    std::cout << "\t\t.ascii   \"";

		  print_escaped_string (obj->get_data_at (addr), size - zt);

		  std::cout << "\"\n";

		  addr += size;
		  len -= size;
		}
	      else
		{
		  char buffer[8];

		  if (bytes_in_line == 0)
		    std::cout << "\t\t.ascii  \"";

		  snprintf (buffer, sizeof (buffer), "\\x%02x",
			    *obj->get_data_at (addr));
		  std::cout << buffer;

		  bytes_in_line += 1;
		  
		  if (bytes_in_line == 8)
		    {
		      std::cout << "\"\n";
		      bytes_in_line = 0;
		    }

		  addr++;
		  len--;
		}
	    }
	}

      if (bytes_in_line > 0)
	std::cout << "\"\n";

      break;

    case Region::VTABLE:
      uint32_t func_addr;
      const Label *next_label;

      /* TODO: limit by relocs */

      print_label (anal->get_label (addr));
      next_label = anal->get_next_label (addr);

      while (addr < reg->get_end_address ())
	{
	  if (next_label != NULL and addr == next_label->get_address ())
	    {
	      print_label (next_label);
	      next_label = anal->get_next_label (addr);
	    }

	  func_addr = read_le<uint32_t> (obj->get_data_at (addr));

	  if (func_addr != 0)
	    {
	      label = anal->get_label (func_addr);
	      assert (label != NULL);
	      std::cout << "\t\t.long   " << *label << "\n";
	    }
	  else
	    std::cout << "\t\t.long   0\n";

	  addr += 4;
	}
      break;

    default:
      break;
    }
}

static void
print_code (LinearExecutable *le, Image *img, Analyser *anal)
{
  enum Section
  {
    NONE,
    TEXT,
    DATA
  };

  Analyser::RegionMap::const_iterator itr;
  const Analyser::RegionMap *regions;
  const Region *prev = NULL;
  const Region *next;
  const Region *reg;
  const Object *obj;
  Section sec = NONE;

  regions = anal->get_regions ();

  std::cerr << "Region count: " << regions->size () << "\n";

  for (itr = regions->begin (); itr != regions->end (); ++itr)
    {
      reg = &itr->second;
      obj = img->get_object_at_address (reg->get_address ());

      if (reg->get_type () == Region::DATA)
	{
	  if (sec != DATA)
	    {
	      std::cout << ".data\n";
	      sec = DATA;
	    }
	}
      else
	{
	  if (sec != TEXT)
	    {
	      std::cout << ".text\n";
	      sec = TEXT;
	    }
	}

      print_region (reg, obj, le, img, anal);

      if (prev != NULL)
	assert (prev->get_end_address () <= reg->get_address ());

      next = anal->get_next_region (reg);
      if (next == NULL or next->get_address () > reg->get_end_address ())
	{
	  const Label *l;

	  l = anal->get_label (reg->get_end_address ());
	  if (l != NULL)
	    print_label (l);
	}

      prev = reg;
    }
}

void
debug_print_regions (Analyser *anal)
{
  const Analyser::RegionMap *map;
  Analyser::RegionMap::const_iterator itr;

  map = anal->get_regions ();

  std::cout << "---------- Regions -------------\n";

  for (itr = map->begin (); itr != map->end (); ++itr)
    std::cout << itr->second << "\n";
}
  
int
main (int argc, char **argv)
{
  LinearExecutable *le;
  Image *image;
  std::ifstream ifs;
  Analyser anal;

  if (argc < 2)
    {
      std::cerr << "Usage: " << argv[0] << " [main.exe]\n";
      return 1;
    }

  ifs.open (argv[1], std::ios::binary);
  if(!ifs.is_open()) {
    std::cerr << "Error opening file: " << argv[1];
    return 1;
  }

  le = LinearExecutable::load (&ifs, argv[1]);

  image = create_image (&ifs, le);

  anal = Analyser (le, image);
  anal.insert_region (Region (0x0e581e,   0x76, Region::DATA));
  anal.insert_region (Region (0x0e5af1,    0xf, Region::DATA));
  anal.insert_region (Region (0x0e73e2,   0x4e, Region::DATA));
  anal.insert_region (Region (0x0ea128,  0x202, Region::DATA));
  anal.insert_region (Region (0x10ae19,   0x25, Region::DATA));
  anal.insert_region (Region (0x10aeb5,   0x25, Region::DATA));
  anal.insert_region (Region (0x117830,  0x200, Region::DATA));
  anal.insert_region (Region (0x1233f3,   0x40, Region::DATA));
  anal.insert_region (Region (0x12b3d0, 0x2450, Region::DATA));
  anal.set_label (Label (0x03cd08, Label::JUMP));
  anal.set_label (Label (0x03fdc8, Label::JUMP));
  anal.set_label (Label (0x035644, Label::JUMP));
  anal.set_label (Label (0x13c443, Label::JUMP));
  anal.set_label (Label (0x140096, Label::FUNCTION));

#include "func_labels.cpp"

  anal.run ();
  anal.remove_label (0x10000);

  print_code (le, image, &anal);

  delete image;
  delete le;
  return 0;
}
