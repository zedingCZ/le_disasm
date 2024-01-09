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

#include "known_file.hpp"
#include "analyser.hpp"
#include "label.hpp"
#include "regions.hpp"

#include "le.hpp"

void
KnownFile::check(Analyser &anal, LinearExecutable *le)
{
  const LinearExecutable::Header *header = le->get_header();

  anal.known_type = KnownFile::NOT_KNOWN;

  if (header->eip_offset == 0xd581c &&
      header->esp_offset == 0x9ffe0 &&
      header->last_page_size == 0x34a &&
      header->fixup_section_size == 0x5d9ca &&
      header->loader_section_size == 0x5df3f &&
      header->object_count == 4)
    {
      if (le->get_object_header(0)->virtual_size == 0x12d030 &&
          le->get_object_header(0)->base_address == 0x10000 &&
          le->get_object_header(1)->virtual_size == 0x96 &&
          le->get_object_header(1)->base_address == 0x140000 &&
          le->get_object_header(2)->virtual_size == 0x9ffe0 &&
          le->get_object_header(2)->base_address == 0x150000 &&
          le->get_object_header(3)->virtual_size == 0x1b58 &&
          le->get_object_header(3)->base_address == 0x1f0000)
        {
          anal.known_type = KnownFile::KNOWN_SWARS_FINAL_MAIN;
          return;
        }
    }
}

void
KnownFile::pre_anal_fixups_apply(Analyser &anal)
{
  switch (anal.known_type)
    {
    case KnownFile::KNOWN_SWARS_FINAL_MAIN:
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
#     include "known_labels_swars.cpp"
      break;
    case KnownFile::NOT_KNOWN:
      break;
    }
}

void
KnownFile::post_anal_fixups_apply(Analyser &anal)
{
  switch (anal.known_type)
    {
    case KnownFile::KNOWN_SWARS_FINAL_MAIN:
      anal.remove_label (0x10000);
      break;
    case KnownFile::NOT_KNOWN:
      break;
    }
}
