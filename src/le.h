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
#ifndef SWDISASM_LE_H
#define SWDISASM_LE_H

#include <inttypes.h>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "util.h"

class LinearExecutable
{
public:
  struct Fixup;

  typedef std::map<uint32_t, Fixup> FixupMap;
  typedef std::set<uint32_t>        AddressSet;

  struct Header
  {
    /* "LE" signature comes before the header data */
    Endianness byte_order;				/* 02h */
    Endianness word_order;				/* 03h */
    uint32_t   format_version;				/* 04h */
    uint16_t   cpu_type;				/* 08h */
    uint16_t   os_type;					/* 0Ah */
    uint32_t   module_version;				/* 0Ch */
    uint32_t   module_flags;				/* 10h */
    uint32_t   page_count;				/* 14h */
    uint32_t   eip_object_index;			/* 18h */
    uint32_t   eip_offset;				/* 1Ch */
    uint32_t   esp_object_index;			/* 20h */
    uint32_t   esp_offset;				/* 24h */
    uint32_t   page_size;				/* 28h */
    uint32_t   last_page_size;				/* 2Ch */
    uint32_t   fixup_section_size;			/* 30h */
    uint32_t   fixup_section_check_sum;			/* 34h */
    uint32_t   loader_section_size;			/* 38h */
    uint32_t   loader_section_check_sum;		/* 3Ch */
    uint32_t   object_table_offset;			/* 40h */
    uint32_t   object_count;				/* 44h */
    uint32_t   object_page_table_offset;		/* 48h */
    uint32_t   object_iterated_pages_offset;		/* 4Ch */
    uint32_t   resource_table_offset;			/* 50h */
    uint32_t   resource_entry_count;			/* 54h */
    uint32_t   resident_name_table_offset;		/* 58h */
    uint32_t   entry_table_offset;			/* 5Ch */
    uint32_t   module_directives_offset;		/* 60h */
    uint32_t   module_directives_count;			/* 64h */
    uint32_t   fixup_page_table_offset;			/* 68h */
    uint32_t   fixup_record_table_offset;		/* 6Ch */
    uint32_t   import_module_name_table_offset;		/* 70h */
    uint32_t   import_module_name_entry_count;		/* 74h */
    uint32_t   import_procedure_name_table_offset;	/* 78h */
    uint32_t   per_page_check_sum_table_offset;		/* 7Ch */
    uint32_t   data_pages_offset;			/* 80h */
    uint32_t   preload_pages_count;			/* 84h */
    uint32_t   non_resident_name_table_offset;		/* 88h */
    uint32_t   non_resident_name_entry_count;		/* 8Ch */
    uint32_t   non_resident_name_table_check_sum;	/* 90h */
    uint32_t   auto_data_segment_object_index;		/* 94h */
    uint32_t   debug_info_offset;			/* 98h */
    uint32_t   debug_info_size;				/* 9Ch */
    uint32_t   instance_pages_count;			/* A0h */
    uint32_t   instance_pages_demand_count;		/* A4h */
    uint32_t   heap_size;				/* A8h */
  };

  struct ObjectHeader
  {
    enum
    {
      READABLE    = 1 << 0,
      WRITABLE    = 1 << 1,
      EXECUTABLE  = 1 << 2,
      RESOURCE    = 1 << 3,
      DISCARDABLE = 1 << 4,
      SHARED      = 1 << 5,
      PRELOADED   = 1 << 6,
      INVALID     = 1 << 7
    };

    uint32_t   virtual_size;				/* 00h */
    uint32_t   base_address;				/* 04h */
    uint32_t   flags;					/* 08h */
    uint32_t   first_page_index;			/* 0Ch */
    uint32_t   page_count;				/* 10h */
    uint32_t   reserved;				/* 14h */
  };

  enum ObjectPageType
  {
    LEGAL       = 0,
    ITERATED    = 1,
    INVALID     = 2,
    ZERO_FILLED = 3,
    LAST        = 4
  };

  struct ObjectPageHeader
  {
    uint16_t   first_number;				/* 00h */
    uint8_t    second_number;				/* 02h */
    ObjectPageType type;				/* 03h */
  };

  struct Fixup
  {
    uint32_t   offset;
    uint32_t   address;
  };

protected:
  class Loader;
  friend class Loader;

protected:
  Header                        header;
  std::vector<ObjectHeader>     objects;
  std::vector<ObjectPageHeader> object_pages;
  std::vector<FixupMap>         fixups;
  AddressSet                    fixup_addresses;

public:
  const Header           *get_header (void) const;
  const FixupMap         *get_fixups_for_object (size_t index) const;
  const AddressSet       *get_fixup_addresses (void) const;
  size_t                  get_object_count (void) const;
  const ObjectHeader     *get_object_header (size_t index) const;
  const ObjectHeader     *get_object_header_at_address (uint32_t addr) const;
  const ObjectPageHeader *get_page_header (size_t index) const;
  size_t                  get_page_file_offset (size_t index) const;

  static LinearExecutable *load (std::istream *is,
				 const std::string &name = "stream");
};

std::ostream &operator<< (std::ostream &os,
			  const LinearExecutable::Header &hdr);

std::ostream &operator<< (std::ostream &os,
			  const LinearExecutable::ObjectHeader &hdr);

std::ostream &operator<< (std::ostream &os,
			  const LinearExecutable::ObjectPageHeader &hdr);

std::ostream &operator<< (std::ostream &os,
			  const LinearExecutable::ObjectPageType &type);

#endif
