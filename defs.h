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

#ifndef _defs_h_
#define _defs_h_

#define PLAYER_CHECK_TOP_MARGIN 16
#define MAX_NAMELEN 14

typedef enum
{
  PLAYER_FACE_UNDEFINED = 0,
  PLAYER_FACE_DOWN,
  PLAYER_FACE_LEFT,
  PLAYER_FACE_RIGHT,
  PLAYER_FACE_UP
} PLAYER_DIR;

typedef enum
{
  ANIM_UP, ANIM_DOWN
} ANIM_DIR;

typedef struct {
  char name[MAX_NAMELEN];
  int hp, hp_max;
  int mp, mp_max;
  int range;
  int reach;
  int alive;
} STATUS;

typedef enum {
  SPELL_TYPE_UNDEFINED = 0,
  SPELL_TYPE_ATTACK,
  SPELL_TYPE_HEAL
} SPELL_TYPE;

typedef struct {
  char name[MAX_NAMELEN];
  SPELL_TYPE type;
  int mp;
  int know;
  int power;
  int reach;
} SPELL_STATUS;

typedef enum {
  ACTOR_NONE = 0,
  ACTOR_HERO,
  ACTOR_ENEMY
} ACTOR_TYPE;

typedef enum {
  BATTLE_STATE_IDLE = 0,
  BATTLE_STATE_WAIT,
  BATTLE_STATE_MOVE,
  BATTLE_STATE_WALK,
  BATTLE_STATE_ATTACK,
  BATTLE_STATE_STRIKE,
  BATTLE_STATE_SELECT_MAGIC,
  BATTLE_STATE_TARGET,
  BATTLE_STATE_DONE
} BATTLE_STATE;

#endif /* _defs_h */

