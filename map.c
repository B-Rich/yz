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

/*
 * map.c -- tiled map format
 */

#include <stdio.h>
#include <stdlib.h>
#include "map.h"

static void clear_map_tiles(Map *map)
{
  int i;

  for (i = 0; i < map->num_tiles; i++) {

    map->tiles[i].index = i;
    map->tiles[i].type = 0;
    map->tiles[i].animated = 0;
    map->tiles[i].start_index = map->tiles[i].end_index = 0;
    map->tiles[i].counter = map->tiles[i].treshold = 0;

  }
}

Map* new_map(int tw, int th, int num_tiles, int w, int h)
{
  Map *map;

  map = (Map *) malloc( sizeof(Map) );
  if (map == NULL)
    return NULL;

  map->tiles = (Tile *) malloc( sizeof(Tile) * num_tiles);
  if (map->tiles == NULL) {

    free(map);
    return NULL;

  }

  map->tw = tw;
  map->th = th;
  map->num_tiles = num_tiles;
  map->w = w;
  map->h = h;
  map->size = w * h;
  map->dat = (int *) malloc( sizeof(map->dat[0]) * map->size);

  clear_map_tiles(map);

  return map;
}

void free_map(Map *m)
{
  free(m->dat);
  free(m->tiles);
  free(m);
}

void clear_map(Map *map, int tile_num)
{
  int i;

  for (i = 0; i < map->size; i++)
    map->dat[i] = tile_num;
}

void puttile_map(Map *m,int x,int y,int t)
{
  m->dat[x/m->tw+y/m->th*m->w]=t;
}

unsigned int gettile_map(Map *m,int x,int y)
{
  return(m->dat[x/m->tw+y/m->th*m->w]);
}

unsigned int gettile_type_map(Map *m,int x,int y)
{
  return(m->tiles[(int)(m->dat[x/m->tw+y/m->th*m->w])].type);
}

unsigned int getblock_index_map(Map *m,int x,int y)
{
  return(m->tiles[(int)(m->dat[x+y*m->w])].index);
}

unsigned int getblock_type_map(Map *m,int x,int y)
{
  return(m->tiles[(int)(m->dat[x+y*m->w])].type);
}

void update_animations_map(Map *m)
{
  static int dir = 1;
  int i;

  for(i=0;i<m->num_tiles;i++)
  {
    switch(m->tiles[i].animated)
    {
      case 0 :
      break;

      case 1 :
      if (++m->tiles[i].counter < m->tiles[i].treshold)
        break;
      m->tiles[i].counter = 0;

      m->tiles[i].index++;

      if(m->tiles[i].index<m->tiles[i].start_index)
        m->tiles[i].index=m->tiles[i].end_index;

      else if(m->tiles[i].index>m->tiles[i].end_index)
	m->tiles[i].index=m->tiles[i].start_index;

      break;

      case 2 :
      if (++m->tiles[i].counter < m->tiles[i].treshold)
        break;
      m->tiles[i].counter = 0;

      if(m->tiles[i].index<=m->tiles[i].start_index)
        dir=1;

      else if(m->tiles[i].index>=m->tiles[i].end_index)
	dir=-1;

      m->tiles[i].index+=dir;

      break;
    }
  }
}

