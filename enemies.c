#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "strike.h"
#include "battle.h"
#include "game.h"

void init_enemies(
  BATTLE *battle
  )
{
  int i, j, n, x, y;
  ACTOR *actor;
  BATTLE_ACTOR *ba = NULL;
  FIELD *field = battle->world->lastField;

  actor = load_actor_XML("hirsh.stat");
  if (actor == NULL) {

    fprintf(stderr, "Error - Unable to load enemy.\n");
    exit(1);

  }

  n = rand() % field->level + 1;
  for (i = 0; i < n; i++) {
    j = 0;
    do {
      x = rand() % field->w;
      y = rand() % field->h;

      if (get_battle_actor_at(battle, ba, field->x + x, field->y + y, 0) == NULL) {
        ba = new_battle_actor(battle, field->x + x, field->y + y, actor, ACTOR_HERO);
        listAdd(&battle->battleList, &ba->listNode);
        j = 10;
      }
      else {
        j++;

#ifdef DEBUG
        printf("Retry #%d to place enemy\n", j);
#endif

      }
    } while (j < 10);
  }

#ifdef DEBUG
  printf("%d battle actor(s) in list.\n", listCount(&battle->battleList));
#endif
}

