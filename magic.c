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
 * magic.c -- Magic for turn based battle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <expat.h>

#include "defs.h"
#include "sprite.h"
#include "magic.h"

static void start(void *data, const char *el, const char **attr)
{
  int i;
  SPELL *dat;

  dat = (SPELL *) data;

  if ( strcmp(el, "spell") == 0) {
    for (i = 0; attr[i]; i += 2) {
      if ( strcmp(attr[i], "name") == 0)
	strcpy(dat->status.name, attr[i + 1]);

      else if ( strcmp(attr[i], "sprite") == 0)
	strcpy(dat->img_fn, attr[i + 1]);
      else if ( strcmp(attr[i], "width") == 0)
	dat->w = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "height") == 0)
	dat->h = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "frames") == 0)
	dat->num_frames = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "treshold") == 0)
	dat->treshold = atoi(attr[i + 1]);

      else if ( strcmp(attr[i], "type") == 0) {
        if ( strcmp(attr[i + 1], "attack") == 0) {
          dat->status.type = SPELL_TYPE_ATTACK;
        }
        else if ( strcmp(attr[i + 1], "heal") == 0) {
          dat->status.type = SPELL_TYPE_HEAL;
        }
        else {
          dat->status.type = SPELL_TYPE_UNDEFINED;
        }
      }

      else if ( strcmp(attr[i], "mp") == 0)
	dat->status.mp = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "know") == 0)
	dat->status.know = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "power") == 0)
	dat->status.power = atoi(attr[i + 1]);
      else if ( strcmp(attr[i], "reach") == 0)
	dat->status.reach = atoi(attr[i + 1]);
    }
  }
}

static void end(void *data, const char *el)
{
}

SPELL *load_spell_XML(const char *fn)
{
  FILE *fp;
  char buff[8192];
  int len, done;
  SPELL *dat;
  XML_Parser p;

  fp = fopen(fn, "rt");
  if (fp == NULL) {
    fprintf(stderr, "ERROR - Unable to load spell %s\n", fn);
    return NULL;
  }

  p = XML_ParserCreate(NULL);
  if (!p) {
    fprintf(stderr, "ERROR - Unable to initialize XML parser for %s\n", fn);
    fclose(fp);
    return NULL;
  }

  if ((dat=(SPELL *)malloc(sizeof(SPELL))) == NULL) {
    fprintf(stderr, "ERROR - Unable to allocate memory for spell %s\n", fn);
    XML_ParserFree(p);
    fclose(fp);
    return NULL;
  }

  XML_SetUserData(p, (void *) dat);
  XML_SetElementHandler(p, start, end);

  for (;;) {

    len = fread(buff, sizeof(buff[0]), 8192, fp);
    if ( ferror(fp) ) {
      fprintf(stderr, "ERROR - File read error for spell %s\n", fn);
      XML_ParserFree(p);
      fclose(fp);
      free(dat);
      return NULL;
    }

    done = feof(fp);

    if (XML_Parse(p, buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr, "XML Parse error at line %d:\n%s\nIn spell file %s\n",
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

  dat->counter = 0;
  dat->curr_frame = 0;

  return dat;
}

void destroy_spell(SPELL *spell)
{
  free_sprite(spell->img);
  free(spell);
}

int is_spell_support(SPELL *spell)
{
  switch(spell->status.type) {
    case SPELL_TYPE_HEAL:
      return 1;
    break;

    default:
    break;
  }

  return 0;
}

int move_spell(SPELL *spell)
{
  if (++spell->counter == spell->treshold) {
    spell->counter = 0;

    if (++spell->curr_frame >= spell->num_frames) {
      spell->curr_frame = 0;
      return 1;
    }

  }

  return 0;
}

void draw_spell(int x, int y, SPELL *spell)
{
  draw_sprite(x, y,
	      spell->curr_frame, spell->img,
	      0, 0, window_width, window_height);
}

