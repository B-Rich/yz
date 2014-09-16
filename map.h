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

/*
 * map.h -- tiled map format
 * header for map.c
 */

#ifndef _map_h
#define _map_h

typedef struct	{
  int index;
  int type;
  int animated;
  int start_index,end_index;
  int counter,treshold;
} Tile;

typedef struct	{
  int  tw,th;
  int w,h;
  int num_tiles,size;
  int  *dat;
  Tile *tiles;
} Map;

typedef struct
{
  int x,y;
  int  w,h;
  int index;
} Object;

extern Map* new_map(int tw, int th, int num_tiles, int w, int h);
extern void free_map(Map *m);
extern void clear_map(Map *m, int tile_num);
extern void puttile_map(Map *m,int x,int y,int t);
extern unsigned int gettile_map(Map *m,int x,int y);
extern unsigned int gettile_type_map(Map *m,int x,int y);
extern unsigned int getblock_index_map(Map *m,int x,int y);
extern unsigned int getblock_type_map(Map *m,int x,int y);
extern void update_animations_map(Map *m);

#endif

