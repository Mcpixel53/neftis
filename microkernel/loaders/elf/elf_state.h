/*
 *    The ELF loader for Atomik microkernel - opaque state
 *    Copyright (C) 2014 Gonzalo J. Carracedo <BatchDrake@gmail.com>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _ELF_STATE_H
#define _ELF_STATE_H

struct elf32_state
{
  Elf32_Ehdr *header;
  Elf32_Phdr *phdrs;
  uint32_t    size;
  int         dyn;
  uint32_t    addr;
  const char *abi_string; /* Static string */
};

struct elf32_note
{
  uint32_t namesz;
  uint32_t descsz;
  uint32_t type;
  char     data[0];
};

#endif /* _ELF_STATE_H */

