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

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "ctrl.h"
#include "sprite.h"
#include "map.h"
#include "draw_text.h"
#include "actor.h"
#include "magic.h"
#include "list.h"
#include "world.h"
#include "player.h"
#include "strike.h"
#include "battle.h"
#include "game.h"
#include "menu.h"

static MENU_TYPE currMenu = MENU_GAME;
static int iGameMenu = 1;
static int iBattleMenu = 1;
static int iSpellMenu = 1;

FONT *menu_fnt;

int init_menu(void)
{
  menu_fnt = load_font("fntdag.png", 8, 8);
  if (menu_fnt == NULL) {
    printf("ERROR - Unable to load menu font.");
    exit(1);
  }

  return 1;
}

void free_menu(void)
{
  free_font(menu_fnt);
}

void set_menu(MENU_TYPE type)
{
  currMenu = type;
}

MENU_TYPE get_menu(void)
{
  return currMenu;
}

void move_menu_option(int *index, int num_choices,
		      MENU_CHOICE_TYPE dir,
		      void (*cb)(int))
{
  if (dir == MENU_DIR_FWD) {
    ++(*index);

    if (*index > num_choices)
      *index = 1;
  }
  else if (dir == MENU_DIR_BACK) {
    --(*index);
    if (*index < 1)
      *index = num_choices;
  }
  else if (dir == MENU_CHOICE_ACCEPT) {
    (*cb)(*index);
  }
  else if (dir == MENU_CHOICE_REGRET) {
    (*cb)(-(*index));
  }
}

static void game_menu_cb(int index)
{
  if (index < 0) {
    set_game_state(GAME_STATE_WORLD);
    return;
  }

  switch (index) {
    case GAME_MENU_NEW    : deinit_game();
			    init_game();
		            break;
    case GAME_MENU_MAP    : set_game_state(GAME_STATE_MAP);
			    break;
    case GAME_MENU_STATUS : set_game_state(GAME_STATE_STATUS);
			    break;
    case GAME_MENU_QUIT   : deinit_game();
			    free_game();
                            exit(0);
		            break;
    default	          : break;
  }
}

static void battle_menu_cb(int index)
{
  if (index < 0) {
    set_game_state(GAME_STATE_WORLD);
    return;
  }

  switch (index) {
    case BATTLE_MENU_MOVE   : set_curr_battle_state(game_battle,
                                                    BATTLE_STATE_MOVE,
                                                    NULL);
		              break;
    case BATTLE_MENU_ATTACK : set_curr_battle_state(game_battle,
                                                    BATTLE_STATE_ATTACK,
                                                    NULL);
		              break;
    case BATTLE_MENU_MAGIC :  set_curr_battle_state(game_battle,
                                                    BATTLE_STATE_SELECT_MAGIC,
                                                    NULL);
		              break;
    case BATTLE_MENU_DEFEND : set_curr_battle_state(game_battle,
                                                    BATTLE_STATE_DONE,
                                                    NULL); 
		              break;
    default	            : break;
  }
}

static void spell_menu_cb(int index)
{
  if (index < 0) {
    set_game_state(GAME_STATE_WORLD);
    return;
  }

  set_curr_battle_state(game_battle, BATTLE_STATE_TARGET, (void*) (index - 1));
}

void move_menu_keydown(int key)
{
  MENU_CHOICE_TYPE dir;
  int input;

  input = get_input_keydown(key);

  if (input & PRESS_DOWN)
    dir = MENU_DIR_FWD;

  else if (input & PRESS_RIGHT)
    dir = MENU_DIR_FWD;

  else if (input & PRESS_UP)
    dir = MENU_DIR_BACK;

  else if (input & PRESS_LEFT)
    dir = MENU_DIR_BACK;

  else if (input & PRESS_ENTER)
    dir = MENU_CHOICE_ACCEPT;

  else if (input & PRESS_ESC)
    dir = MENU_CHOICE_REGRET;

  else
    dir = MENU_CHOICE_NONE;

  switch (currMenu) {
    case MENU_GAME    : move_menu_option(&iGameMenu,
				         GAME_MENU_NUM_CHOICES,
				         dir,
				         game_menu_cb);
		        break;
    case MENU_BATTLE  : move_menu_option(&iBattleMenu,
				         BATTLE_MENU_NUM_CHOICES,
				         dir,
				         battle_menu_cb);
                        break;
    case MENU_MAGIC   : move_menu_option(&iSpellMenu,
				         PLAYER_MAX_SPELLS,
				         dir,
				         spell_menu_cb);
                        break;
    default:		break;
  }

}

void draw_menu(int x, int y)
{
  char str[(MAX_NAMELEN + 4) * PLAYER_MAX_SPELLS];

  switch (currMenu) {
    case MENU_GAME   : draw_menu_box(x, y,
				     "New Game\n"
				     "Map\n"
				     "Status\n"
				     "Quit",
				     iGameMenu,
				     0,
				     menu_fnt);
		       break;
    case MENU_BATTLE : draw_menu_box(x, y,
				     "Move\n"
				     "Attack\n"
				     "Magic\n"
				     "Defend",
				     iBattleMenu,
				     0,
				     menu_fnt);
		       break;
    case MENU_MAGIC  : list_curr_battle_spells(game_battle, str);
                       draw_menu_box(x, y, str, iSpellMenu, 0, menu_fnt);
                       break;
    default 	     : break;
  }
}

