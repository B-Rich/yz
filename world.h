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

#ifndef _world_h
#define _world_h

#define WORLD_TILE_WIDTH		32
#define WORLD_TILE_HEIGHT		32
#define WORLD_NUM_TILES			5

#define WORLD_EMPTY_TILE		1
#define WORLD_FIELD_TILE		0
#define WORLD_ROAD_TILE			0
#define WORLD_BLOCK_TILE_LO		2
#define WORLD_BLOCK_TILE_HI		3

struct _PLAYER;

typedef enum {
  ROAD_ORI_HORIZONTAL = 0,
  ROAD_ORI_VERTICAL
} ROAD_ORI;

typedef enum {
  WORLD_SPACE_BLOCK = -1,
  WORLD_EMPTY_BLOCK = 0,
  WORLD_ROAD_BLOCK_S = 1,
  WORLD_ROAD_BLOCK_H,
  WORLD_ROAD_BLOCK_HS,
  WORLD_ROAD_BLOCK_HE,
  WORLD_ROAD_BLOCK_V,
  WORLD_ROAD_BLOCK_VS,
  WORLD_ROAD_BLOCK_VE,
  WORLD_ROAD_BLOCK_E,
  WORLD_FIELD_BLOCK,
  WORLD_DONE_BLOCK
} WORLD_BLOCK_TYPE;

typedef enum {
  FIELD_POS_N = 0,
  FIELD_POS_NE,
  FIELD_POS_E,
  FIELD_POS_SE,
  FIELD_POS_S,
  FIELD_POS_SW,
  FIELD_POS_W,
  FIELD_POS_NW
} FIELD_POS;

typedef struct fieldType {
  LIST_NODE listNode;
  int id;
  int x, y;
  int w, h;
  int level;
} FIELD;

typedef struct {
  WORLD_BLOCK_TYPE *world_map;
  SPRITE *tiles;
  Map *map;
  int tile_base;
  int tile_width, tile_height;
  int x, y;
  int width, height;
  LIST fieldList;
  FIELD *lastField;
} WORLD;

int reveal_world_terrain(
  WORLD *world,
  int x,
  int y
  );

WORLD* new_world(
  char *spr,
  int tile_index,
  int x,
  int y,
  int w,
  int h,
  int cw,
  int ch
  );

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
  );

void draw_world(
  WORLD *world
  );

void free_world(
  WORLD *world
  );

void move_world(
  WORLD *world,
  struct _PLAYER *player
  );

void draw_world(
  WORLD *world
  );

int check_world_tile(
  WORLD *world,
  int index,
  SPRITE *spr,
  int x,
  int y
  );

#endif

