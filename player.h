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

#ifndef _player_h
#define _player_h

#define PLAYER_DEFAULT_SPEED 6
#define PLAYER_DEFAULT_TRESHOLD 4
#define PLAYER_MAX_SPELLS 10

typedef struct _PLAYER
{
   PLAYER_DIR dir;
   int x, y;
   int frame, counter, treshold;
   ANIM_DIR anim_dir;
   int speed;

   ACTOR *h;
   SPELL *spells[PLAYER_MAX_SPELLS];
} PLAYER;

PLAYER* new_player(
  int x,
  int y,
  ACTOR *actor
  );

void init_player(
  PLAYER *player,
  int x,
  int y,
  ACTOR *actor
  );

void free_player(
  PLAYER *player
  );

int move_player(
  PLAYER *player,
  WORLD *world
  );

int move_player_to(
  PLAYER *player,
  int x,
  int y
  );

void draw_player(
  PLAYER *player,
  WORLD *world
  );

SPELL* get_player_spell(
  PLAYER *player,
  int index
  );

char* list_player_spells(
  PLAYER *player,
  char *str
  );

#endif

