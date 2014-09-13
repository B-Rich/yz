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
#include "sprite.h"
#include "map.h"
#include "draw_text.h"
#include "menu.h"
#include "world.h"
#include "scanner.h"

SCANNER* new_scanner(
  char *spr
  )
{
  SCANNER *scanner;

  scanner = (SCANNER *) malloc( sizeof(SCANNER) );
  if (scanner != NULL) {
    if (!init_scanner(scanner, spr)) {
      free(scanner);
      scanner = NULL;
    }
  }

  return scanner;
}

int init_scanner(
  SCANNER *scanner,
  char *spr
  )
{
  scanner->marker=load_sprite(spr, SCANNER_MARKER_SIZE, SCANNER_MARKER_SIZE);
  if (scanner->marker == NULL) {
    fprintf(stderr, "Unable to load scanner marker %s\n", spr);
    return 0;
  }

  return 1;
}

void free_scanner(
  SCANNER *scanner
  )
{
  free_sprite(scanner->marker);
  free(scanner);
}

void draw_scanner(
  SCANNER *scanner,
  WORLD *world,
  int x,
  int y,
  int px,
  int py
  )
{
  int i, j, t;

  draw_text_background(x, y,
		       SCANNER_MARKER_SIZE * world->map->w,
		       SCANNER_MARKER_SIZE * world->map->h,
		       menu_fnt);
  for (j = 0; j < world->map->h; j++) {

    for (i = 0; i < world->map->w; i++) {

      switch(world->world_map[i + j * world->map->w]) {

        case WORLD_SPACE_BLOCK: t = 3; break;
        case WORLD_ROAD_BLOCK_HS: t = 12; break;
        case WORLD_ROAD_BLOCK_H: t = 12; break;
        case WORLD_ROAD_BLOCK_HE: t = 12; break;
        case WORLD_ROAD_BLOCK_VS: t = 12; break;
        case WORLD_ROAD_BLOCK_V: t = 12; break;
        case WORLD_ROAD_BLOCK_VE: t = 12; break;
        case WORLD_FIELD_BLOCK: t = 12; break;
        case WORLD_DONE_BLOCK: t = 2; break;
        default: t = 0; break;

      }

      if (t)
        draw_sprite(i * SCANNER_MARKER_SIZE + x + menu_fnt->img->w,
		    j * SCANNER_MARKER_SIZE + y + menu_fnt->img->h,
		    t, scanner->marker,
		    0, 0, window_width, window_height);

    }

  }

  draw_sprite((px / world->tile_width) * SCANNER_MARKER_SIZE + x +
              menu_fnt->img->w,
	      (py / world->tile_height) * SCANNER_MARKER_SIZE + y +
              menu_fnt->img->h,
	      12, scanner->marker,
	      0, 0, window_width, window_height);
}

