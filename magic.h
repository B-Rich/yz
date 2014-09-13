/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of yz.
*   Copyright (C) 2013 Surplus Users Ham Society
*
*   Yz is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 2 of the License, or
*   (at your option) any later version.
*
*   Yz is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with Yz.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/*
 * magic.h -- Magic for turn based fight
 */

#ifndef _magic_h
#define _magic_h

typedef struct {
  char img_fn[80];
  SPELL_STATUS status;
  SPRITE *img;
  int w, h;
  int num_frames;
  int counter, treshold;
  int curr_frame;
} SPELL;

extern SPELL *load_spell_XML(const char *fn);
extern void destroy_spell(SPELL *spell);
extern int is_spell_support(SPELL *spell);
extern int move_spell(SPELL *spell);
extern void draw_spell(int x, int y, SPELL *spell);

#endif

