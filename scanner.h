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

#ifndef _scanner_h
#define _scanner_h

#define SCANNER_MARKER_SIZE		2

typedef struct {
  SPRITE *marker;
} SCANNER;

SCANNER* new_scanner(
  char *spr
  );

int init_scanner(
  SCANNER *scanner,
  char *spr
  );

void free_scanner(
  SCANNER *scanner
  );

void draw_scanner(
  SCANNER *scanner,
  WORLD *world,
  int x,
  int y,
  int px,
  int py
  );

#endif

