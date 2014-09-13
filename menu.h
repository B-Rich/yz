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

#ifndef _menu_h
#define _menu_h

#define GAME_MENU_NUM_CHOICES 4
#define GAME_MENU_NEW 1
#define GAME_MENU_MAP 2
#define GAME_MENU_STATUS 3
#define GAME_MENU_QUIT 4

#define BATTLE_MENU_NUM_CHOICES 4
#define BATTLE_MENU_MOVE 1
#define BATTLE_MENU_ATTACK 2
#define BATTLE_MENU_MAGIC 3
#define BATTLE_MENU_DEFEND 4

typedef enum {
  MENU_NONE = 0,
  MENU_GAME,
  MENU_BATTLE,
  MENU_MAGIC
} MENU_TYPE;

typedef enum {
  MENU_CHOICE_NONE = 0,
  MENU_DIR_FWD,
  MENU_DIR_BACK,
  MENU_CHOICE_ACCEPT,
  MENU_CHOICE_REGRET
} MENU_CHOICE_TYPE;

extern FONT *menu_fnt;

extern int init_menu(void);
extern void free_menu(void);
extern void set_menu(MENU_TYPE type);
extern MENU_TYPE get_menu(void);
extern void move_menu_keydown(int key);
extern void draw_menu(int x, int y);

#endif

