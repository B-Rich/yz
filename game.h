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

#define DEFAULT_CELL_WIDTH	16
#define DEFAULT_CELL_HEIGHT	16

typedef enum {
  GAME_STATE_WORLD = 0,
  GAME_STATE_MENU,
  GAME_STATE_MAP,
  GAME_STATE_STATUS,
  GAME_STATE_BATTLE
} GAME_STATE_TYPE;

extern WORLD *game_world;
extern BATTLE *game_battle;
extern ACTOR *game_hero;
extern PLAYER *game_player;

int load_game(void);
void free_game(void);
void init_game(void);
void deinit_game(void);
void set_game_state(GAME_STATE_TYPE s);
GAME_STATE_TYPE get_game_state(void);
void update_game_keydown(int key);
void update_game(void);
void draw_game(void);

