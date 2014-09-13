/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of yz.
*   Copyright (C) 2010 Surplus Users Ham Society
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "sprite.h"
#include "map.h"
#include "draw_text.h"
#include "menu.h"
#include "strike.h"

STRIKE* new_strike(
  void
  )
{
  STRIKE *strike;

  strike = (STRIKE *) malloc( sizeof(STRIKE) );
  if (strike != NULL) {
    if (!init_strike(strike)) {
      free(strike);
      strike = NULL;
    }
  }

  return strike;
}

int init_strike(
  STRIKE *strike
  )
{
  memset(strike, 0, sizeof(STRIKE));
  strike->font = menu_fnt;

  return 1;
}

void free_strike(
  STRIKE *strike
  )
{
  free(strike);
}

int prepare_strike(
  STRIKE *strike,
  int points,
  int x,
  int y,
  int h
  )
{
  strike->points = points;
  strike->x = x;
  strike->y = y;
  strike->pos_y = 0;
  strike->end_y = h;

  return 1;
}

int move_strike(
  STRIKE *strike
  )
{
  int result = 0;

  if (++strike->counter == STRIKE_DEFAULT_TRESHOLD) {
    strike->counter = 0;
    strike->pos_y += STRIKE_TEXT_FALL_SPEED;
    if (strike->pos_y >= strike->end_y) {
      result = strike->points;
    }
  }

  return result;
}

void draw_strike(
  STRIKE *strike
  )
{
  char str[4];

  sprintf(str, "%-4d", strike->points);
  draw_text_line(strike->x, strike->y + strike->pos_y, str, strike->font);
}

