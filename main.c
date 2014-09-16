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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL/SDL.h>

#include "defs.h"
#include "sprite.h"
#include "map.h"
#include "draw_text.h"
#include "list.h"
#include "world.h"
#include "actor.h"
#include "magic.h"
#include "player.h"
#include "strike.h"
#include "battle.h"
#include "game.h"
#include "fps_timer.h"

static SDL_Surface *screen;
static int screen_width = DEFAULT_WINDOW_WIDTH;
static int screen_height = DEFAULT_WINDOW_HEIGHT;

int init(void)
{
  /* Initlaize SDL with video and sound if possible */
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Fatal Error -- Unable to initialize SDL: %s\n", SDL_GetError());
    return 0;
  }

  /* Install exit function */
  atexit(SDL_Quit);

  /* Initialize screen, setup gfx mode */
  screen = SDL_SetVideoMode(screen_width, screen_height, 32,
                            SDL_HWSURFACE|SDL_DOUBLEBUF);
  if (screen == NULL) {
    fprintf(stderr, "Fatal Error -- Unable to set video mode: %s\n",
            SDL_GetError());
    return 0;
  }

  load_game();
  init_game();
  set_sprite_context(screen, screen_width, screen_height);

  return 1;
}

void redraw(void)
{
  update_game();
  draw_game();
}

void flip(void)
{
  SDL_Flip(screen);
}

int main(int argc, char *argv[])
{
  SDL_Event event;
  int done = 0;

  if (!init())
    return 1;

  while (!done) {

    if (fps_timer(60)) {
    
      while (SDL_PollEvent(&event) ) {

        if (event.type == SDL_QUIT) done = 1;
        if (event.type == SDL_KEYDOWN)
          update_game_keydown(event.key.keysym.sym);

      }

      redraw();
      flip();

    }

  }

  return 0;
}

