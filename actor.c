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
 * actor.c -- Game actors
 */

#include <stdlib.h>
#include <string.h>

#include <expat.h>

#include "defs.h"
#include "sprite.h"
#include "draw_text.h"
#include "actor.h"
#include "menu.h"

static void start(void *data, const char *el, const char **attr)
{
  int i;
  ACTOR *dat;

  dat = (ACTOR *) data;

  if ( strcmp(el, "actor") == 0) {
    for (i = 0; attr[i]; i += 2) {
      if ( strcmp(attr[i], "name") == 0)
	strcpy(dat->status.name, attr[i + 1]);

      if ( strcmp(attr[i], "sprite") == 0)
	strcpy(dat->img_fn, attr[i + 1]);
      else if ( strcmp(attr[i], "width") == 0)
	dat->w = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "height") == 0)
	dat->h = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "treshold") == 0)
	dat->treshold = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "startdown") == 0)
	dat->start_frame_down = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "standdown") == 0)
	dat->stand_frame_down = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "enddown") == 0)
	dat->end_frame_down = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "startleft") == 0)
	dat->start_frame_left = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "standleft") == 0)
	dat->stand_frame_left = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "endleft") == 0)
	dat->end_frame_left = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "startright") == 0)
	dat->start_frame_right = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "standright") == 0)
	dat->stand_frame_right = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "endright") == 0)
	dat->end_frame_right = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "startup") == 0)
	dat->start_frame_up = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "standup") == 0)
	dat->stand_frame_up = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "endup") == 0)
	dat->end_frame_up = atoi(attr[i + 1]);

      else if ( strcmp(attr[i], "hp") == 0)
	dat->status.hp_max = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "mp") == 0)
	dat->status.mp_max = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "range") == 0)
        dat->status.range = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "reach") == 0)
        dat->status.reach = atoi(attr[i + 1]);
    }
  }
}

static void end(void *data, const char *el)
{
}

ACTOR *load_actor_XML(const char *fn)
{
  FILE *fp;
  char buff[8192];
  int len, done;
  ACTOR *dat;
  XML_Parser p;

  fp = fopen(fn, "rt");
  if (fp == NULL) {
    fprintf(stderr, "ERROR - Unable to load actor %s\n", fn);
    return NULL;
  }

  p = XML_ParserCreate(NULL);
  if (!p) {
    fprintf(stderr, "ERROR - Unable to initialize XML parser for %s\n", fn);
    fclose(fp);
    return NULL;
  }

  if ((dat=(ACTOR *)malloc(sizeof(ACTOR))) == NULL) {
    fprintf(stderr, "ERROR - Unable to allocate memory for actor %s\n", fn);
    XML_ParserFree(p);
    fclose(fp);
    return NULL;
  }

  XML_SetUserData(p, (void *) dat);
  XML_SetElementHandler(p, start, end);

  for (;;) {

    len = fread(buff, sizeof(buff[0]), 8192, fp);
    if ( ferror(fp) ) {
      fprintf(stderr, "ERROR - File read error for actor %s\n", fn);
      XML_ParserFree(p);
      fclose(fp);
      free(dat);
      return NULL;
    }

    done = feof(fp);

    if (XML_Parse(p, buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr, "XML Parse error at line %d:\n%s\nIn actor file %s\n",
              (int)XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)),
	      fn);
      XML_ParserFree(p);
      fclose(fp);
      free(dat);
      return NULL;
    }

    if (done)
      break;
  }

  XML_ParserFree(p);
  fclose(fp);

  if ((dat->img=load_sprite(dat->img_fn,
	dat->w, dat->h)) == NULL) {
    free(dat);
    fprintf(stderr, "ERROR - Unable to load sprite %s\n", dat->img_fn);
    return NULL;
  }

  dat->status.hp = dat->status.hp_max;
  dat->status.mp = dat->status.mp_max;
  dat->status.alive = 1;

  dat->counter = 0;
  dat->hit = 0;
  dat->index = -1;

  return dat;
}

void free_actor(ACTOR *actor)
{
  free(actor);
}

int actor_alive(ACTOR *actor)
{
  return actor->status.alive;
}

void draw_actor_status(int x, int y, ACTOR *actor)
{
  char str[255];

  sprintf(str, "%-14s\n"
	       " \n \n \n \n \n \n \n"
	       "Hp : %-4d/%-4d\n"
               "Mp : %-4d/%-4d\n"
               "Range:    %-4d\n"
               "Reach:    %-4d",
	       actor->status.name,
	       actor->status.hp, actor->status.hp_max,
	       actor->status.mp, actor->status.mp_max,
               actor->status.range,
               actor->status.reach);

  draw_text_box(x, y, str, menu_fnt);
  draw_sprite(x + 8, y + 32, actor->stand_frame_down, actor->img,
	      0, 0, window_width, window_height);
}

