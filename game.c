/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of yz.
*   Copyright (C) 2014 Surplus Users Ham Society
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
#include <time.h>

#include "defs.h"
#include "ctrl.h"
#include "sprite.h"
#include "map.h"
#include "draw_text.h"
#include "menu.h"
#include "actor.h"
#include "magic.h"
#include "list.h"
#include "world.h"
#include "player.h"
#include "scanner.h"
#include "strike.h"
#include "battle.h"
#include "enemies.h"
#include "game.h"

WORLD *game_world;
SCANNER *game_scanner;
BATTLE *game_battle;
ACTOR *game_hero;
PLAYER *game_player;

static GAME_STATE_TYPE state = GAME_STATE_WORLD;

int load_game(void)
{
  srand(time(0));
  init_menu();

  return 1;
}

void free_game(void)
{
  free_menu();
}

void init_game(int tile_index)
{
  if ((game_hero = load_actor_XML("aron.stat")) == NULL) {
    fprintf(stderr, "ERROR - Unable to initialize hero.\n");
    exit(1);
  }

  if ((game_player = new_player(128 * 32, 96 * 32, game_hero)) == NULL) {
    fprintf(stderr, "ERROR - Unable to initialize player.\n");
    exit(1);
  }

  if ((game_world = new_world("world.png", tile_index,
                         game_player->x, game_player->y,
                         300, 200,
	                 DEFAULT_CELL_WIDTH, DEFAULT_CELL_HEIGHT)) == NULL) {
    fprintf(stderr, "ERROR - Unable to initialize world\n");
    exit(1);
  }

  if ((game_scanner = new_scanner("marker4x4.png")) == NULL) {
    fprintf(stderr, "ERROR - Unable to initialize scanner\n");
    exit(1);
  }

  if ((game_battle = new_battle(game_world)) == NULL) {
    fprintf(stderr, "ERROR - Unable to initialize battle\n");
    exit(1);
  }

  set_game_state(GAME_STATE_WORLD);

  update_game();
}

void deinit_game(void)
{
  free_battle(game_battle);
  free_scanner(game_scanner);
  free_world(game_world);
  free_actor(game_hero);
}

void set_game_state(GAME_STATE_TYPE s)
{
  state = s;

  switch(state) {
    case GAME_STATE_MENU   : set_menu(MENU_GAME);
			     break;
    case GAME_STATE_BATTLE : set_menu(MENU_BATTLE);
                             init_battle_player(game_battle, game_player);
                             init_enemies(game_battle);
			     break;
    default		   : break;
  }
}

GAME_STATE_TYPE get_game_state(void)
{
  return state;
}

static void check_esc(int key)
{
  int input = get_input_keydown(key);

  if (input & PRESS_ESC) {

    switch(state) {

      case GAME_STATE_WORLD  : set_game_state(GAME_STATE_MENU);
			       break;
      case GAME_STATE_MAP    : set_game_state(GAME_STATE_WORLD);
			       break;
      case GAME_STATE_STATUS : set_game_state(GAME_STATE_WORLD);
			       break;
      case GAME_STATE_BATTLE : deinit_battle(game_battle);
			       set_game_state(GAME_STATE_WORLD);
			       break;
      default                : break;

    }

  }
}

void update_game_keydown(int key)
{
  switch(state) {
    case GAME_STATE_MENU   : move_menu_keydown(key);
			     break;
    case GAME_STATE_BATTLE : check_esc(key);
			     move_battle_keydown(game_battle, key);
			     break;
    default		   : check_esc(key);
			     break;
  }
}

void update_game(void)
{
  switch(state) {
    case GAME_STATE_WORLD  : if (move_player(game_player, game_world))
                               set_game_state(GAME_STATE_BATTLE);
 			     break;
    case GAME_STATE_MENU   : break;
    case GAME_STATE_BATTLE : move_battle(game_battle);
			     break;
    default		   : break;
  }
}

void draw_game(void)
{
  switch(state) {
    case GAME_STATE_WORLD  : draw_world(game_world);
			     draw_player(game_player, game_world);
 			     break;
    case GAME_STATE_MENU   : draw_menu(10, 10);
			     break;
    case GAME_STATE_MAP    : draw_scanner(game_scanner, game_world, 10, 10,
                                          game_player->x, game_player->y);
			     break;
    case GAME_STATE_STATUS : draw_actor_status(10, 10, game_player->h);
			     break;
    case GAME_STATE_BATTLE : draw_world(game_world);
			     draw_battle(game_battle);
			     break;
    default		   : break;
  }
}

