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

static int set_player_dir(
  PLAYER *player,
  PLAYER_DIR dir
  )
{
  if (dir == player->dir)
    return 0;

  player->counter = 0;
  player->anim_dir = ANIM_UP;
  switch(dir) {
    case PLAYER_FACE_DOWN:
      player->frame = player->h->start_frame_down;
      player->dir = dir;
      break;

    case PLAYER_FACE_LEFT:
      player->frame = player->h->start_frame_left;
      player->dir = dir;
      break;

    case PLAYER_FACE_RIGHT:
      player->frame = player->h->start_frame_right;
      player->dir = dir;
      break;

    case PLAYER_FACE_UP:
      player->frame = player->h->start_frame_up;
      player->dir = dir;
      break;

    default:
       player->dir = PLAYER_FACE_UNDEFINED;
       break;
  }

  return 1;
}

PLAYER* new_player(
  int x,
  int y,
  ACTOR *actor
  )
{
  PLAYER *player;
  player = (PLAYER *) malloc( sizeof(PLAYER) );
  if (player != NULL)
    init_player(player, x, y, actor);

  return player;
}

void init_player(
  PLAYER *player,
  int x,
  int y,
  ACTOR *actor
  )
{
  memset(player, 0, sizeof(PLAYER));
  player->x = x;
  player->y = y;
  player->treshold = PLAYER_DEFAULT_TRESHOLD;
  player->speed = PLAYER_DEFAULT_SPEED;
  player->h = actor;
  set_player_dir(player, PLAYER_FACE_DOWN);
}

void free_player(
  PLAYER *player
  )
{
  free(player);
}

static int set_player_stand(
  PLAYER *player
  )
{
  int done = 0;

  if (++player->counter == player->treshold) {
    player->counter = 0;
    done = 1;
    switch(player->dir) {
      case PLAYER_FACE_DOWN:
        player->frame = player->h->stand_frame_down;
        break;

      case PLAYER_FACE_LEFT:
        player->frame = player->h->stand_frame_left;
        break;

      case PLAYER_FACE_RIGHT:
        player->frame = player->h->stand_frame_right;
        break;

      case PLAYER_FACE_UP:
        player->frame = player->h->stand_frame_up;
        break;

      default:
        player->frame = player->h->stand_frame_down;
        break;
    }
  }

  return done;
}

static void animate_player(
  PLAYER *player
  )
{
  if (++player->counter == player->treshold) {
    player->counter = 0;
    switch(player->dir) {
      case PLAYER_FACE_DOWN:
        if (player->anim_dir == ANIM_UP) {
          if (++player->frame == player->h->end_frame_down)
            player->anim_dir = ANIM_DOWN;
        }
        else if (player->anim_dir == ANIM_DOWN) {
          if (--player->frame == player->h->start_frame_down)
            player->anim_dir = ANIM_UP;
        }
        break;

      case PLAYER_FACE_LEFT:
        if (player->anim_dir == ANIM_UP) {
          if (++player->frame == player->h->end_frame_left)
            player->anim_dir = ANIM_DOWN;
        }
        else if (player->anim_dir == ANIM_DOWN) {
          if (--player->frame == player->h->start_frame_left)
            player->anim_dir = ANIM_UP;
        }
        break;

      case PLAYER_FACE_RIGHT:
        if (player->anim_dir == ANIM_UP) {
          if (++player->frame == player->h->end_frame_right)
            player->anim_dir = ANIM_DOWN;
        }
        else if (player->anim_dir == ANIM_DOWN) {
          if (--player->frame == player->h->start_frame_right)
            player->anim_dir = ANIM_UP;
        }
        break;

      case PLAYER_FACE_UP:
        if (player->anim_dir == ANIM_UP) {
          if (++player->frame == player->h->end_frame_up)
            player->anim_dir = ANIM_DOWN;
        }
        else if (player->anim_dir == ANIM_DOWN) {
          if (--player->frame == player->h->start_frame_up)
            player->anim_dir = ANIM_UP;
        }
        break;

        default:
          player->anim_dir = ANIM_UP;
          break;
    }
  }
}

int get_player_speed_down(
  PLAYER *player,
  WORLD *world
  )
{
  int j1, j2;
  int speed;

  for (j1 = player->speed; j1 > 0; j1--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x + 1,
                         player->y + player->h->h + j1))
      break;
  }

  for (j2 = player->speed; j2 > 0; j2--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x + player->h->w - 1,
                         player->y + player->h->h + j2))
      break;
  }

  speed = (j1 < j2) ? (j1) : (j2);

  return speed;
}

int get_player_speed_left(
  PLAYER *player,
  WORLD *world
  )
{
  int j1, j2;
  int speed;

  for (j1 = player->speed; j1 > 0; j1--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x - j1,
                         player->y + PLAYER_CHECK_TOP_MARGIN + 2))
      break;
  }

  for (j2 = player->speed; j2 > 0; j2--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x - j2,
                         player->y + player->h->h - 1))
      break;
  }

  speed = (j1 < j2) ? (j1) : (j2);

  return speed;
}

int get_player_speed_right(
  PLAYER *player,
  WORLD *world
  )
{
  int j1, j2;
  int speed;

  for (j1 = player->speed; j1 > 0; j1--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x + player->h->w + j1,
                         player->y + PLAYER_CHECK_TOP_MARGIN + 2))
      break;
  }

  for (j2 = player->speed; j2 > 0; j2--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x + player->h->w + j2,
                         player->y + player->h->h - 1))
      break;
  }

  speed = (j1 < j2) ? (j1) : (j2);

  return speed;
}

int get_player_speed_up(
  PLAYER *player,
  WORLD *world
  )
{
  int j1, j2;
  int speed;

  for (j1 = player->speed; j1 > 0; j1--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x + 1,
                         player->y + PLAYER_CHECK_TOP_MARGIN - j1))
      break;
  }

  for (j2 = player->speed; j2 > 0; j2--) {
    if (!check_world_tile(world,
                         player->frame, player->h->img,
                         player->x + player->h->w - 1,
                         player->y + PLAYER_CHECK_TOP_MARGIN - j2))
      break;
  }

  speed = (j1 < j2) ? (j1) : (j2);

  return speed;
}

static int reveal_player_world(
  PLAYER *player,
  WORLD *world
  )
{
  int reveal = 0;

  switch(player->dir) {
    case PLAYER_FACE_DOWN:
      reveal = reveal_world_terrain(world,
                                    player->x,
                                    player->y + player->h->h + 1);
      if (!reveal)
        reveal = reveal_world_terrain(world,
                                      player->x + player->h->w,
                                      player->y + player->h->h + 1);
      break;

    case PLAYER_FACE_LEFT:
      reveal = reveal_world_terrain(world,
                                    player->x - 1,
                                    player->y + PLAYER_CHECK_TOP_MARGIN);
      if (!reveal)
        reveal = reveal_world_terrain(world,
                                      player->x - 1,
                                      player->y + player->h->h);
      break;

    case PLAYER_FACE_RIGHT:
      reveal = reveal_world_terrain(world,
                                    player->x + player->h->w + 1,
                                    player->y + PLAYER_CHECK_TOP_MARGIN);
      if (!reveal)
        reveal = reveal_world_terrain(world,
                                      player->x + player->h->w + 1,
                                      player->y + player->h->h);
      break;

    case PLAYER_FACE_UP:
      reveal = reveal_world_terrain(world,
                                    player->x,
                                    player->y - 1);
      if (!reveal)
        reveal = reveal_world_terrain(world,
                                      player->x + player->h->w,
                                      player->y - 1);
      break;

    default:
      break;
  }

  return reveal;
}

int move_player(
  PLAYER *player,
  WORLD *world
  )
{
  int reveal = 0;
  int input = get_input();

  if (input & PRESS_DOWN) {
    set_player_dir(player, PLAYER_FACE_DOWN);
    animate_player(player);
    reveal = reveal_player_world(player, world);
    player->y += get_player_speed_down(player, world);
  }

  else if (input & PRESS_LEFT) {
    set_player_dir(player, PLAYER_FACE_LEFT);
    animate_player(player);
    reveal = reveal_player_world(player, world);
    player->x -= get_player_speed_left(player, world);
  }

  else if (input & PRESS_RIGHT) {
    set_player_dir(player, PLAYER_FACE_RIGHT);
    animate_player(player);
    reveal = reveal_player_world(player, world);
    player->x += get_player_speed_right(player, world);
  }

  else if (input & PRESS_UP) {
    set_player_dir(player, PLAYER_FACE_UP);
    animate_player(player);
    reveal = reveal_player_world(player, world);
    player->y -= get_player_speed_up(player, world);
  }
  else {
    set_player_stand(player);
  }

  move_world(world, player);

  return reveal;
}

static int move_player_down(
  PLAYER *player,
  int r
  )
{
  int done = 0;

  if (player->y < r) {
    set_player_dir(player, PLAYER_FACE_DOWN);
    animate_player(player);
    player->y += player->speed;
  }

  if (player->y >= r) {
    player->y = r;
    done = 1;
  }

  return done;
}

static int move_player_left(
  PLAYER *player,
  int r
  )
{
  int done = 0;

  if (player->x > r) {
    set_player_dir(player, PLAYER_FACE_LEFT);
    animate_player(player);
    player->x -= player->speed;
  }

  if (player->x <= r) {
    player->x = r;
    done = 1;
  }

  return done;
}

static int move_player_right(
  PLAYER *player,
  int r
  )
{
  int done = 0;

  if (player->x < r) {
    set_player_dir(player, PLAYER_FACE_RIGHT);
    animate_player(player);
    player->x += player->speed;
  }

  if (player->x >= r) {
    player->x = r;
    done = 1;
  }

  return done;
}

static int move_player_up(
  PLAYER *player,
  int r
  )
{
  int done = 0;

  if (player->y > r) {
    set_player_dir(player, PLAYER_FACE_UP);
    animate_player(player);
    player->y -= player->speed;
  }

  if (player->y <= r) {
    player->y = r;
    done = 1;
  }

  return done;
}

int move_player_to(
  PLAYER *player,
  int x,
  int y
  )
{
  int done = 0;

  if (player->y < y)
    move_player_down(player, y);
  else if (player->x > x)
    move_player_left(player, x);
  else if (player->x < x)
    move_player_right(player, x);
  else if (player->y > y)
    move_player_up(player, y);
  else
    done = set_player_stand(player);

  return done;
}

void draw_player(
  PLAYER *player,
  WORLD *world
  )
{
  draw_sprite(player->x - world->x,
              player->y - world->y,
              player->frame, player->h->img,
              0, 0, window_width, window_height);
}

SPELL* get_player_spell(
  PLAYER *player,
  int index
  )
{
  SPELL *spell = NULL;

  if (player->spells[index] != NULL)
    spell = player->spells[index];

  return spell;
}

char* list_player_spells(
  PLAYER *player,
  char *str
  )
{
  int i;

  strcpy(str, "");
  for (i = 0; i < PLAYER_MAX_SPELLS; i++) {
    if (player->spells[i] != NULL) {
      strcat(str, player->spells[i]->status.name);
      strcat(str, "\n");
    }
    else {
      strcat(str, "--- Empty ---\n");
    }
  }

  return str;
}

