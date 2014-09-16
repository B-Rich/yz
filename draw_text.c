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
 * draw_text.c -- 2d text engine
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sprite.h"
#include "draw_text.h"

FONT *load_font(const char *fn, int w, int h)
{
  FONT *fnt;

  /* Allocate memory for struct */
  fnt = malloc(sizeof(FONT));
  if (fnt == NULL) {
    fprintf(stderr, "Fatal Error -- Unable to load font %s\n", fn);
    return NULL;
  }

  /* Load image with all glyphs */
  fnt->img = load_sprite(fn, w, h);
  if (fnt->img == NULL)
  {
    free(fnt);
    return NULL;
  }

  return fnt;
}

void free_font(FONT *fnt)
{
  free_sprite(fnt->img);
}

void draw_text_line(int x, int y, const char *msg, FONT *fnt)
{
  int i, len;
  int dx, dy, fw, fh;

  /* Cache properties */
  len = strlen(msg);
  dx = x;
  dy = y;
  fw = fnt->img->w;
  fh = fnt->img->h;

  /* Draw a sprite for each character */
  for (i = 0; i < len; i++) {
    draw_sprite(dx, dy, msg[i] - ' ', fnt->img,
		0, 0, window_width, window_height);
    dx += fw;
  }

}

static int get_strings(char **str, char *msg, int *width, FONT *fnt)
{
  char *token;
  char tmp[255];
  int w, fw, max_width = 0, n = 0;

  /* Separate a continuous string to multiple strings ending at newlines */

  /* Find first newline token */
  strcpy(tmp, msg);
  token = strtok(tmp, "\n");
  fw = fnt->img->w;

  /* Separate string at newline tokens */
  while (token != NULL) {

    w = fw * strlen(token);
    if (max_width < w) max_width = w;

    str[n] = malloc(strlen(token));
    strcpy(str[n], token);
    n++;

    token = strtok(NULL, "\n");
  }

  *width = max_width;
  return n;
}

static void free_strings(char **str, int n)
{
  int i;

  /* Free separated strings */
  for (i = 0; i < n; i++)
    free(str[i]);
}

void draw_text_background(int x, int y, int w, int h, FONT *fnt)
{
  int i, j, fw, fh;

  /* Get propertices */
  fw = fnt->img->w;
  fh = fnt->img->h;

  /* Draw four corners for the box */
  draw_sprite(x, y, TEXTBOX_TOPLEFT_SPRITE, fnt->img,
	      0, 0, window_width, window_height);
  draw_sprite(x + w + fw, y, TEXTBOX_TOPRIGHT_SPRITE, fnt->img,
	      0, 0, window_width, window_height);
  draw_sprite(x, y + h + fh, TEXTBOX_BOTTOMLEFT_SPRITE, fnt->img,
	      0, 0, window_width, window_height);
  draw_sprite(x + w + fw, y + h+ fh, TEXTBOX_BOTTOMRIGHT_SPRITE, fnt->img,
	      0, 0, window_width, window_height);

  /* Draw left column */
  for (i = fh; i <= h; i += fh)
    draw_sprite(x, y + i, TEXTBOX_LEFT_SPRITE, fnt->img,
	        0, 0, window_width, window_height);

  /* Draw top row */
  for (i = fw; i <= w; i += fw)
    draw_sprite(x + i, y, TEXTBOX_TOP_SPRITE, fnt->img,
	        0, 0, window_width, window_height);

  /* Draw right column */
  for (i = fh; i <= h; i += fh)
    draw_sprite(x + w + fw, y + i, TEXTBOX_RIGHT_SPRITE, fnt->img,
	        0, 0, window_width, window_height);

  /* Draw bottom row */
  for (i = fw; i <= w; i += fw)
    draw_sprite(x + i, y + h + fh, TEXTBOX_BOTTOM_SPRITE, fnt->img,
	        0, 0, window_width, window_height);

  /* Draw background */
  for (j = fh; j <= h; j += fh)
    for (i = fw; i <= w; i += fw)
      draw_sprite(x + i, y + j, TEXTBOX_BACKGROUND_SPRITE, fnt->img,
	          0, 0, window_width, window_height);

}

static void draw_menu_foreground(int x, int y, int r, int c, FONT *fnt)
{
  draw_sprite( x + fnt->img->w * c, y + fnt->img->h * r, TEXTBOX_LEFTARROW_SPRITE, fnt->img,
	      0, 0, window_width, window_height);
}

void draw_text_box(int x, int y, char *msg, FONT *fnt)
{
  int i, n;
  int xp, yp, width, height;
  char *str[64];

  /* Store font height */
  height = fnt->img->h;

  /* Cut string to multiple strings ending at newline tokens */
  /* Width will be equal to total maximal length of string lines in pixels */
  n = get_strings(str, msg, &width, fnt);

  draw_text_background(x, y, width, height * n, fnt);

  /* Recalculate positions */
  xp = x + fnt->img->w;
  yp = y + fnt->img->h;

  /* Draw text strings */
  for ( i = 0; i < n; i++, yp += height) {
    draw_text_line(xp, yp, str[i], fnt);
  }

  /* Free temporary strings */
  free_strings(str, n);
}

void draw_menu_box(int x, int y, char *msg, int r, int c, FONT *fnt)
{
  int i, n;
  int xp, yp, width, height;
  char *str[64];

  /* Store font height */
  height = fnt->img->h;

  /* Cut string to multiple strings ending at newline tokens */
  /* Width will be equal to total maximal length of string lines in pixels */
  n = get_strings(str, msg, &width, fnt);

  draw_text_background(x, y, width, height * n, fnt);

  /* Recalculate positions */
  xp = x + fnt->img->w;
  yp = y + fnt->img->h;

  /* Draw text strings */
  for ( i = 0; i < n; i++, yp += height) {
    draw_text_line(xp, yp, str[i], fnt);
  }

  /* Draw marker */
  draw_menu_foreground(x, y, r, c, fnt);

  /* Free temporary strings */
  free_strings(str, n);
}

