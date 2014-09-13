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

#include "defs.h"
#include "list.h"
#include "pixel.h"
#include "sprite.h"
#include "biot.h"
#include "map.h"
#include "draw_map.h"
#include "draw_text.h"
#include "menu.h"
#include "actor.h"
#include "magic.h"
#include "world.h"
#include "player.h"
#include "strike.h"
#include "battle.h"
#include "game.h"

static void put_world_tile(
  WORLD *world,
  int tx,
  int ty,
  int tile,
  int unique,
  int reveal
  )
{
  WORLD_BLOCK_TYPE v;
  int x, y, offs;

  offs = tx + ty * world->map->w;

  if (unique) {

    v = world->world_map[offs];
    if (v)
      return;

  }

  x = tx * world->tile_width;
  y = ty * world->tile_height;

  puttile_map(world->map, x, y, world->tile_base + tile);

  if (reveal)
    world->world_map[offs] = WORLD_DONE_BLOCK;
  else
    world->world_map[offs] = WORLD_SPACE_BLOCK;
}

static FIELD* add_world_field(
  WORLD *world,
  int tx,
  int ty,
  int tw,
  int th
  )
{
  static int id;
  FIELD *field;
  int i, j, ie, je;

  if (tx < 2 || tx > world->map->w - 2 ||
      ty < 2 || ty > world->map->h - 2)
    return NULL;

  if (tx + tw > world->map->w - 2 ||
      ty + th > world->map->h - 2)
    return NULL;

  field = (FIELD *) malloc( sizeof(FIELD) );
  if (field == NULL) {

    fprintf(stderr, "Error - Unable to allocate field\n");
    exit(1);

  }

  field->x = tx;
  field->y = ty;
  field->w = tw;
  field->h = th;
  field->level = tw * th / 8;
  field->id = id++;

  listInsert(&world->fieldList, NULL, &field->listNode);

  ie = field->x + field->w;
  je = field->y + field->h;

  for (j = field->y; j < je; j ++)
    for (i = field->x; i < ie; i ++)
      world->world_map[i + j * world->map->w] = WORLD_FIELD_BLOCK;

  return field;
}

static void reveal_world_field(
  WORLD *world,
  FIELD *field
  )
{
  int i, j, ie, je;
  ie = field->x + field->w;
  je = field->y + field->h;

  /* Top row */
  for (i = field->x; i < ie; i++) {

    put_world_tile(world, i, field->y - 2, WORLD_BLOCK_TILE_HI, 1, 0);
    put_world_tile(world, i, field->y - 1, WORLD_BLOCK_TILE_LO, 1, 0);

  }

  /* Left column */
  for (j = field->y - 2; j < je + 1; j++)
    put_world_tile(world, field->x - 1, j, WORLD_BLOCK_TILE_HI, 1, 0);

  /* Right column */
  for (j = field->y - 2; j < je + 1; j++)
    put_world_tile(world, field->x + field->w, j, WORLD_BLOCK_TILE_HI, 1, 0);

  /* Bottom row */
  for (i = field->x; i < ie; i++)
    put_world_tile(world, i, field->y + field->h, WORLD_BLOCK_TILE_HI, 1, 0);

  /* The field itself */
  for (j = field->y; j < je; j++)
    for (i = field->x; i < ie; i++)
      put_world_tile(world, i, j, WORLD_FIELD_TILE, 0, 1);

}

static void add_world_roadpice(
  WORLD *world,
  int tx,
  int ty,
  int te,
  ROAD_ORI ori
  )
{
  int i, j, end;

  if (tx < 2 || tx > world->map->w - 2 ||
      ty < 2 || ty > world->map->h - 2)
    return;

  switch(ori) {

    case ROAD_ORI_HORIZONTAL:

      if (tx + te > world->map->w - 2)
        return;

      world->world_map[tx + ty * world->map->w] = WORLD_ROAD_BLOCK_HS;

      i = tx + 1;
      j = ty;
      end = tx + te - 1;
      for (; i < end; i++)
	world->world_map[i + j * world->map->w] = WORLD_ROAD_BLOCK_H;

      world->world_map[end + ty * world->map->w] = WORLD_ROAD_BLOCK_HE;

    break;

    case ROAD_ORI_VERTICAL:

      if (ty + te > world->map->h - 2)
        return;

      world->world_map[tx + ty * world->map->w] = WORLD_ROAD_BLOCK_VS;

      i = tx;
      j = ty + 1;
      end = ty + te - 1;
      for (; j < end; j++)
	world->world_map[i + j * world->map->w] = WORLD_ROAD_BLOCK_V;

      world->world_map[tx + end * world->map->w] = WORLD_ROAD_BLOCK_VE;

    break;

    default:
    break;

  }

}

void add_world_road(
  WORLD *world,
  int tx,
  int ty,
  int te,
  ROAD_ORI ori
  )
{
  int i, ie;

  switch(ori) {

    case ROAD_ORI_HORIZONTAL:

#ifndef NOROADCLIP

      for (i = tx; i < tx + te; ) {

        while (world->world_map[i + ty * world->map->w] == WORLD_FIELD_BLOCK &&
	       i < tx + te)
          i++;

        ie = 0;
        while (world->world_map[(i + ie) + ty * world->map->w] != WORLD_FIELD_BLOCK &&
	       i + ie < tx + te)
          ie++;

        if (ie >= 2)
          add_world_roadpice(world, i, ty, ie, ori);

        i += ie;

      }
#else

      add_world_roadpice(world, tx, ty, te, ori);

#endif

    break;

    case ROAD_ORI_VERTICAL:

#ifndef NOROADCLIP

      for (i = ty; i < ty + te; ) {

        while (world->world_map[tx + i * world->map->w] == WORLD_FIELD_BLOCK &&
	       i < ty + te)
          i++;

        ie = 0;
        while (world->world_map[tx + (i + ie) * world->map->w] != WORLD_FIELD_BLOCK &&
	       i + ie < ty + te)
          ie++;

        if (ie >= 2)
          add_world_roadpice(world, tx, i, ie, ori);

        i += ie;

      }
#else

      add_world_roadpice(world, tx, ty, te, ori);

#endif

    break;

  }
}

static FIELD_POS get_world_field_pos(
  FIELD *f0,
  FIELD *f1
  )
{
  FIELD_POS pos;
  int f0x, f0y, f1x, f1y;

  f0x = f0->x + f0->w / 2;
  f0y = f0->y + f0->h / 2;

  f1x = f1->x + f1->w / 2;
  f1y = f1->y + f1->h / 2;

  if (f1x == f0x && f1y < f0y)
    pos = FIELD_POS_N;
  else if (f1x > f0x && f1y < f0y)
    pos = FIELD_POS_NE;
  else if (f1x > f0x && f1y == f0y)
    pos = FIELD_POS_E;
  else if (f1x > f0x && f1y > f0y)
    pos = FIELD_POS_SE;
  else if (f1x == f0x && f1y > f0y)
    pos = FIELD_POS_S;
  else if (f1x < f0x && f1y > f0y)
    pos = FIELD_POS_SW;
  else if (f1x < f0x && f1y == f0y)
    pos = FIELD_POS_W;
  else if (f1x < f0x && f1y < f0y)
    pos = FIELD_POS_NW;

  return pos;
}

static void add_world_road_to(
  WORLD *world,
  FIELD *f0,
  FIELD *f1
  )
{
  int f0x, f0y, f1x, f1y;

  f0x = f0->x + f0->w / 2;
  f0y = f0->y + f0->h / 2;

  f1x = f1->x + f1->w / 2;
  f1y = f1->y + f1->h / 2;

  switch (get_world_field_pos(f0, f1)) {

    case FIELD_POS_N:
      add_world_road(world,
                     f0x, f1y + f1->h / 2,
		     f0y - f1y - f0->h / 2 - f1->h / 2,
		     ROAD_ORI_VERTICAL);
    break;

    case FIELD_POS_NE:
      add_world_road(world,
                     f0x + 1, f1y,
		     f1x - f0y - f1->w / 2 - 1,
		     ROAD_ORI_HORIZONTAL);
      add_world_road(world,
                     f0x, f1y,
		     f0y - f1y - f1->h / 2 + 1,
		     ROAD_ORI_VERTICAL);
    break;

    case FIELD_POS_E:
      add_world_road(world,
                     f0x + f0->w / 2, f0y,
		     f1x - f0y - f0->w / 2 - f1->w / 2,
		     ROAD_ORI_HORIZONTAL);
    break;

    case FIELD_POS_SE:
      add_world_road(world,
                     f0x + f0->w / 2, f0y,
		     f1x - f0y - f0->w / 2,
		     ROAD_ORI_HORIZONTAL);
      add_world_road(world,
                     f1x, f0y,
		     f1y - f0y - f1->h / 2,
		     ROAD_ORI_VERTICAL);
    break;

    case FIELD_POS_S:
      add_world_road(world,
                     f0x, f0y + f0->h / 2,
		     f1y - f0y - f0->h / 2 - f1->h / 2,
		     ROAD_ORI_VERTICAL);
    break;

    case FIELD_POS_SW:
      add_world_road(world,
                     f1x + f1->w / 2, f1y,
		     f0x - f1x - f1->w / 2,
		     ROAD_ORI_HORIZONTAL);
      add_world_road(world,
                     f0x, f0y + f0->h / 2,
		     f1y - f0y - f1->h / 2 + 1,
		     ROAD_ORI_VERTICAL);
    break;

    case FIELD_POS_W:
      add_world_road(world,
                     f1x + f1->w / 2, f0y,
		     f0x - f1x - f0->w /2 - f1->w / 2,
		     ROAD_ORI_HORIZONTAL);
    break;

    case FIELD_POS_NW:
      add_world_road(world,
                     f1x + 1, f0y,
		     f0x - f1x - f1->w / 2 - 1,
		     ROAD_ORI_HORIZONTAL);
      add_world_road(world,
                     f1x, f1y + f1->h / 2,
		     f0y - f1y - f1->h / 2 + 1,
		     ROAD_ORI_VERTICAL);
    break;
  }
  
}

static void reveal_world_road(
  WORLD *world,
  int i,
  int j,
  int v
  )
{
  put_world_tile(world, i, j, WORLD_ROAD_TILE, 0, 1);

  switch(v) {

    case WORLD_ROAD_BLOCK_H:
      put_world_tile(world, i, j - 2, WORLD_BLOCK_TILE_HI, 1, 0);
      put_world_tile(world, i, j - 1, WORLD_BLOCK_TILE_LO, 1, 0);
      put_world_tile(world, i, j + 1, WORLD_BLOCK_TILE_HI, 1, 0);
    break;

    case WORLD_ROAD_BLOCK_HS:
      put_world_tile(world, i, j - 2, WORLD_BLOCK_TILE_HI, 0, 0);
      put_world_tile(world, i, j - 1, WORLD_BLOCK_TILE_LO, 0, 0);
      put_world_tile(world, i, j + 1, WORLD_BLOCK_TILE_HI, 0, 0);

      if (!world->world_map[(i - 1) + j * world->map->w]) {

        put_world_tile(world, i - 1, j - 2, WORLD_BLOCK_TILE_HI, 0, 0);
        put_world_tile(world, i - 1, j - 1, WORLD_BLOCK_TILE_HI, 0, 0);
        put_world_tile(world, i - 1, j, WORLD_BLOCK_TILE_HI, 0, 0);
        put_world_tile(world, i - 1, j + 1, WORLD_BLOCK_TILE_HI, 0, 0);

      }

    break;

    case WORLD_ROAD_BLOCK_HE:
      put_world_tile(world, i, j - 2, WORLD_BLOCK_TILE_HI, 0, 0);
      put_world_tile(world, i, j - 1, WORLD_BLOCK_TILE_LO, 0, 0);
      put_world_tile(world, i, j + 1, WORLD_BLOCK_TILE_HI, 0, 0);

      if (!world->world_map[(i + 1) + j * world->map->w]) {

        put_world_tile(world, i + 1, j - 2, WORLD_BLOCK_TILE_HI, 0, 0);
        put_world_tile(world, i + 1, j - 1, WORLD_BLOCK_TILE_HI, 0, 0);
        put_world_tile(world, i + 1, j, WORLD_BLOCK_TILE_HI, 0, 0);
        put_world_tile(world, i + 1, j + 1, WORLD_BLOCK_TILE_HI, 0, 0);

      }

    break;

    case WORLD_ROAD_BLOCK_V:
      put_world_tile(world, i - 1, j, WORLD_BLOCK_TILE_HI, 1, 0);
      put_world_tile(world, i + 1, j, WORLD_BLOCK_TILE_HI, 1, 0);
    break;

    case WORLD_ROAD_BLOCK_VS:
      put_world_tile(world, i - 1, j, WORLD_BLOCK_TILE_HI, 1, 0);
      put_world_tile(world, i + 1, j, WORLD_BLOCK_TILE_HI, 1, 0);

      if (!world->world_map[i + (j - 1) * world->map->w]) {

        put_world_tile(world, i - 1, j - 1, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i, j - 1, WORLD_BLOCK_TILE_LO, 1, 0);
        put_world_tile(world, i + 1, j - 1, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i - 1, j - 2, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i, j - 2, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i + 1, j - 2, WORLD_BLOCK_TILE_HI, 1, 0);

      }

    break;

    case WORLD_ROAD_BLOCK_VE:

      if (world->world_map[i + (j + 1) * world->map->w]) {

        put_world_tile(world, i - 1, j, WORLD_BLOCK_TILE_LO, 1, 0);
        put_world_tile(world, i + 1, j, WORLD_BLOCK_TILE_LO, 1, 0);

      }

      else {

        put_world_tile(world, i - 1, j, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i + 1, j, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i - 1, j + 1, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i, j + 1, WORLD_BLOCK_TILE_HI, 1, 0);
        put_world_tile(world, i + 1, j + 1, WORLD_BLOCK_TILE_HI, 1, 0);

      }

    break;

    default:
    break;
  }
}

int reveal_world_terrain(
  WORLD *world,
  int x,
  int y
  )
{
  FIELD *field;
  int i, j;
  WORLD_BLOCK_TYPE v;

  i = x / world->tile_width;
  j = y / world->tile_height;

  v = world->world_map[i + j * world->map->w];
  if (v > WORLD_EMPTY_BLOCK) {

    if ( (v >= WORLD_ROAD_BLOCK_S) ||
	 (v <= WORLD_ROAD_BLOCK_E) ) {

      reveal_world_road(world, i, j, v);

    }

    if (v == WORLD_FIELD_BLOCK) {

      for(field = (FIELD *) LIST_HEAD(&world->fieldList);
	  field != NULL;
          field = (FIELD *) LIST_NEXT(&field->listNode)) {

        if ( i >= field->x && i <= (field->x + field->w) &&
	     j >= field->y && j <= (field->y + field->h) ) {

          reveal_world_field(world, field);
          world->lastField = field;
	  return 1;

        }

      }

    }

  }

  return 0;

}

static FIELD* process_world_row(
  WORLD *world,
  int row,
  int w,
  int sx,
  int sy,
  int cw,
  int ch,
  int rev
  )
{
  int i, on, sp, fx, fy, fw, fh;
  FIELD *f, *field = NULL;

  if (!rev) {

    for (i = 0; i < w; i++) {

      if (i == sx && row == sy) {

        sp = 1;
        on = 1;

      }

      else {

        sp = 0;
        on = rand() % 2;

      }

      if (on) {

        do {

          fw = rand() % (cw - 4);
          fh = rand() % (ch - 4);

        } while (fw < 4 || fh < 4);

        fx = i * cw - fw / 2;
        fy = row * ch - fh / 2;

        f = add_world_field(world, fx, fy, fw, fh);
        if (sp)
	  field = f;

      }

    }

  }

  else {

    for (i = w; i > 0; i--) {

      if (i == sx && row == sy) {

        sp = 1;
        on = 1;

      }

      else {

        sp = 0;
        on = rand() % 2;

      }

      if (on) {

        do {

          fw = rand() % (cw - 4);
          fh = rand() % (ch - 4);

        } while (fw < 4 || fh < 4);

        fx = i * cw - fw / 2;
        fy = row * ch - fh / 2;

        f = add_world_field(world, fx, fy, fw, fh);
        if (sp)
	  field = f;

      }

    }

  }

  return field;
}

static void init_world_terrain(
  WORLD *world,
  int x,
  int y,
  int cw,
  int ch
  )
{
  int i, sx, sy;
  int mw, mh;
  FIELD *f, *fs, *fm, *fe, *field;

  sx = x / (world->tile_width * cw);
  sy = y / (world->tile_height * ch);

  mw = world->map->w / cw;
  mh = world->map->h / ch;

  for (i = 0; i < mh; i++) {

    f = process_world_row(world, i, mw, sx, sy, cw, ch, (i + 1) % 2);
    if (f != NULL)
      fm = f;

  }

  fs = (FIELD *) LIST_HEAD(&world->fieldList);
  fe = (FIELD *) LIST_TAIL(&world->fieldList);

  for(field = (FIELD *) LIST_TAIL(&world->fieldList);
      field != NULL;
      field = (FIELD *) LIST_PREV(&field->listNode)) {

    f = (FIELD *) LIST_PREV(&field->listNode);
    if (f == NULL)
      break;

    add_world_road_to(world, field, f);
    add_world_road_to(world, field, fs);
    add_world_road_to(world, field, fm);
    add_world_road_to(world, field, fe);
  }

  reveal_world_terrain(world, x, y);
}

static void init_world_map(
  WORLD *world,
  int w,
  int h
  )
{
  int i, size;

  listInit(&world->fieldList);
  world->lastField = NULL;

  size = w * h;
  world->world_map = (WORLD_BLOCK_TYPE *) malloc( sizeof(WORLD_BLOCK_TYPE) * size);
  if (world->world_map == NULL) {

    fprintf(stderr, "Error - Unable to initialize map\n");
    exit(1);

  }

  for (i = 0; i < size; i++)
    world->world_map[i] = WORLD_EMPTY_BLOCK;
}

WORLD* new_world(
  char *spr,
  int tile_index,
  int x,
  int y,
  int w,
  int h,
  int cw,
  int ch
  )
{
  WORLD *world;

  world = (WORLD *) malloc( sizeof(WORLD) );
  if (world != NULL)
    init_world(world, spr, tile_index, x, y, w, h, cw, ch);

  return world;
}

void init_world(
  WORLD *world,
  char *spr,
  int tile_index,
  int x,
  int y,
  int w,
  int h,
  int cw,
  int ch
  )
{
  init_world_map(world, w, h);

  /* Load tiles */
  world->tiles=load_sprite(spr, WORLD_TILE_WIDTH, WORLD_TILE_HEIGHT);
  if (world->tiles == NULL) {

    fprintf(stderr, "Unable to load tiles %s\n", spr);
    exit(1);

  }

  world->map=new_map(WORLD_TILE_WIDTH,
                     WORLD_TILE_HEIGHT,
                     WORLD_NUM_TILES, w, h);
  if (world->map == NULL) {

    free_sprite(world->tiles);
    fprintf(stderr, "Unable to load create new map\n");
    exit(1);

  }

  world->tile_base = tile_index;
  world->tile_width = WORLD_TILE_WIDTH;
  world->tile_height = WORLD_TILE_HEIGHT;
  world->width = world->tile_width * w;
  world->height = world->tile_height * h;

  world->map->tiles[world->tile_base + WORLD_EMPTY_TILE].type = 1;
  world->map->tiles[world->tile_base + WORLD_BLOCK_TILE_LO].type = 1;
  world->map->tiles[world->tile_base + WORLD_BLOCK_TILE_HI].type = 1;

  clear_map(world->map, world->tile_base + WORLD_EMPTY_TILE);
  init_world_terrain(world, x, y, cw, ch);
}

void free_world(
  WORLD *world
  )
{
  FIELD *field;

  while ((field = (FIELD *) LIST_HEAD(&world->fieldList)) != NULL) {

    listRemove(&world->fieldList, (LIST_NODE *) field);
    free(field);

  }

  free(world->world_map);
  free_map(world->map);
  free_sprite(world->tiles);
  free(world);
}

void move_world(
  WORLD *world,
  PLAYER *player
  )
{
  world->x = player->x - window_width / 2;
  world->y = player->y - window_height / 2;
  /* TODO: Check out of bounds */
}

void draw_world(
  WORLD *world
  )
{
  draw_map(0, 0, window_width, window_height,
	   world->map, world->x, world->y, world->tiles);
}

int check_world_tile(
  WORLD *world,
  int index,
  SPRITE *spr,
  int x,
  int y
  )
{
  Tile *tile;
  int tx, ty;

  tile = &world->map->tiles[gettile_map(world->map, x, y)];
  if (tile->type >= 1) {

    tx = (x/world->map->tw)*world->map->tw-world->x;
    ty = (y/world->map->th)*world->map->th-world->y;
    if (biot(index,spr,
	     x-world->x, y-world->y,
	     tile->index, world->tiles,
	     tx, ty) )
      return 1;

  }

  return 0;
}

