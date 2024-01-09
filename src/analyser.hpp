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

#ifndef LEDISASM_ANALYSER_H
#define LEDISASM_ANALYSER_H

#include <deque>
#include <inttypes.h>
#include <map>
#include <string>

#include "disassembler.hpp"
#include "known_file.hpp"

class LinearExecutable;
class Image;
class Label;
class Region;

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
  KnownFile::Type      known_type;

  friend class KnownFile;

protected:
  void  add_region (const Region &reg);

  void  add_initial_regions (void);
  void  add_eip_to_trace_queue (void);
  void  add_code_trace_address (uint32_t addr);

  void  trace_code (void);
  void  trace_code_at_address (uint32_t start_addr);

  Region * get_previous_region (const Region *reg);
  Region * get_region_at_address (uint32_t address);
  Region * get_region (uint32_t address);

  void insert_region (Region *parent, const Region &reg);
  void check_merge_regions (uint32_t addr);

  void trace_vtables (void);
  void trace_remaining_relocs (void);

public:
  Analyser (void);
  Analyser (const Analyser &other);
  Analyser (LinearExecutable *le, Image *img);

  Analyser &operator= (const Analyser &other);

  Label * get_next_label (const Label *lab);
  Label * get_next_label (uint32_t addr);
  Region *get_next_region (const Region *reg);

  void insert_region (const Region &reg);
  void set_label (const Label &lab);
  void remove_label (uint32_t addr);
  void run (void);

  const RegionMap *  get_regions (void) const;
  const LabelMap *  get_labels (void) const;
  const Label *  get_label (uint32_t addr) const;
};

#endif // LEDISASM_ANALYSER_H
