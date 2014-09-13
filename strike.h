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

#ifndef _strike_h
#define _strike_h

#define STRIKE_DEFAULT_TRESHOLD 2
#define STRIKE_TEXT_FALL_SPEED  5

typedef struct {
  FONT *font;
  int counter;
  int points;
  int x, y;
  int pos_y, end_y;
} STRIKE;

STRIKE* new_strike(
  void
  );

int init_strike(
  STRIKE *strike
  );

void free_strike(
  STRIKE *strike
  );

int prepare_strike(
  STRIKE *strike,
  int points,
  int x,
  int y,
  int h
  );

int move_strike(
  STRIKE *strike
  );

void draw_strike(
  STRIKE *strike
  );

#endif

