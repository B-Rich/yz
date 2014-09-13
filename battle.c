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

BATTLE* new_battle(
  WORLD *world
  )
{
  BATTLE *battle;

  battle = (BATTLE *) malloc( sizeof(BATTLE) );
  if (battle != NULL) {
    if (!init_battle(battle, world)) {
      free(battle);
      battle = NULL;
    }
  }

  return battle;
}

int init_battle(
  BATTLE *battle,
  WORLD *world
  )
{
  memset(battle, 0, sizeof(BATTLE));

  listInit(&battle->battleList);
  battle->world = world;
  battle->bmarker = load_sprite("bmarker.png", BMARKER_WIDTH, BMARKER_HEIGHT);
  if (battle->bmarker == NULL) {
    fprintf(stderr, "ERROR - Unable to load battle markers.\n");
    return 0;
  }

  battle->strike = new_strike();
  if (battle->strike == NULL) {
    free_sprite(battle->bmarker);
    fprintf(stderr, "ERROR - Unable to create battle strike.\n");
    return 0;
  }

  return 1;
}

BATTLE_ACTOR* get_battle_actor_at(
  BATTLE *battle,
  BATTLE_ACTOR *curr,
  int i,
  int j,
  int self
  )
{
  BATTLE_ACTOR *ba;

  for (ba = (BATTLE_ACTOR *) LIST_HEAD(&battle->battleList);
       ba != NULL;
       ba = (BATTLE_ACTOR *) LIST_NEXT(&ba->listNode))  {

    if (ba->i == i && ba->j == j && (self || ba != curr)) {

#ifdef DEBUG
      printf("%d actor at: [%d,%d]\n", ba->type, i, j);
#endif

      return ba;

    }

  }

  return NULL;
}

static STEP_POINT* new_battle_step(
  int i,
  int j
  )
{
  STEP_POINT *p;

  p = malloc( sizeof(STEP_POINT) );
  if (p == NULL) {

    fprintf(stderr, "Error - Out of memory.\n");
    exit(1);

  }

  p->i = i;
  p->j = j;

  return p;
}

static STEP_POINT* copy_battle_step(
  STEP_POINT *p
  )
{
  STEP_POINT *q;

  q = malloc( sizeof(STEP_POINT) );
  if (q == NULL) {

    fprintf(stderr, "Error - Out of memory.\n");
    exit(1);

  }

  memcpy(q, p, sizeof(STEP_POINT));

  return q;
}

static int init_battle_steps(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  int i, j, r, count;
  STEP_POINT *p;

  p = new_battle_step(ba->i, ba->j);
  listAdd(&ba->walkList, &p->listNode);

  r = ba->player->h->status.range;

  for (j = -r; j <= r; j++) {

    for (i = -r; i <= r; i++) {

      if (gettile_type_map(battle->world->map,
			   (ba->i + i) * BMARKER_WIDTH,
			   (ba->j + j) * BMARKER_HEIGHT))
        continue;

      if (get_battle_actor_at(battle, ba, ba->i + i, ba->j + j, 0) != NULL)
        continue;

      p = new_battle_step(ba->i + i, ba->j + j);
      listAdd(&ba->stepList, &p->listNode);
      count++;

    }

  }

  ba->cursor.i = ba->i;
  ba->cursor.j = ba->j;

  return count;
}

static void free_battle_steps(
  BATTLE_ACTOR *ba
  )
{
  STEP_POINT *p;

  while ((p = (STEP_POINT *) LIST_HEAD(&ba->stepList)) != NULL) {
    listRemove(&ba->stepList, &p->listNode);
    free(p);
  }

#ifdef DEBUG
    printf("Free battle steps, %d element(s) left\n",
           listCount(&ba->stepList));
#endif
}

static int init_battle_attack(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  int i, j, r, count;
  STEP_POINT *p;

  r = ba->player->h->status.reach;
  count = 0;

  for (j = -r; j <= r; j++) {

    for (i = -r; i <= r; i++) {

      if (get_battle_actor_at(battle, ba, ba->i + i, ba->j + j, 0) != NULL) {

        p = new_battle_step(ba->i + i, ba->j + j);
        listAdd(&ba->attackList, &p->listNode);

        ba->cursor.i = p->i;
        ba->cursor.j = p->j;
        count++;

#ifdef DEBUG
        printf("Enemy within reach at: [%d, %d]\n",
               p->i, p->j);
#endif
      }

    }

  }

  return count;
}

static void free_battle_attack(
  BATTLE_ACTOR *ba
  )
{
  STEP_POINT *p;

  while ((p = (STEP_POINT *) LIST_HEAD(&ba->attackList)) != NULL) {
    listRemove(&ba->attackList, &p->listNode);
    free(p);
  }

#ifdef DEBUG
    printf("Free battle attack, %d element(s) left\n",
           listCount(&ba->attackList));
#endif
}

static int init_battle_target(
  BATTLE *battle,
  BATTLE_ACTOR *ba,
  int self
  )
{
  int i, j, r, count;
  STEP_POINT *p;

#ifdef DEBUG
  printf("Battle spell: %s\n", ((SPELL *) ba->arg)->status.name);
#endif

  r = ((SPELL *) ba->arg)->status.reach;
  count = 0;

  for (j = -r; j <= r; j++) {

    for (i = -r; i <= r; i++) {

      if (get_battle_actor_at(battle, ba, ba->i + i, ba->j + j, self) != NULL) {

        p = new_battle_step(ba->i + i, ba->j + j);
        listAdd(&ba->targetList, &p->listNode);

        ba->cursor.i = p->i;
        ba->cursor.j = p->j;
        count++;

#ifdef DEBUG
        printf("Target within reach at: [%d, %d]\n",
               p->i, p->j);
#endif
      }

    }

  }

  return count;
}

static void free_battle_target(
  BATTLE_ACTOR *ba
  )
{
  STEP_POINT *p;

  while ((p = (STEP_POINT *) LIST_HEAD(&ba->targetList)) != NULL) {
    listRemove(&ba->targetList, &p->listNode);
    free(p);
  }

#ifdef DEBUG
    printf("Free battle target, %d element(s) left\n",
           listCount(&ba->targetList));
#endif
}

static void init_battle_strike(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba = (BATTLE_ACTOR *) battle->currActor->arg;

#ifdef DEBUG
    printf("Character with index: %d striking character with index: %d\n",
           battle->currActor->index, ba->index);
#endif
  prepare_strike(battle->strike, 1,
                 ba->player->x + ba->player->h->w / 2 - battle->world->x,
                 ba->player->y - battle->world->y,
                 ba->player->h->h);
}

static int set_battle_state(
  BATTLE *battle,
  BATTLE_ACTOR *ba,
  BATTLE_STATE state,
  void *arg
  )
{
  SPELL *spell;

  if (arg != NULL) {

#ifdef DEBUG
    printf("Set battle argument: %x\n", (int) arg);
#endif

    ba->arg = arg;

  }

  switch(state) {

    case BATTLE_STATE_WAIT:
      set_menu(MENU_BATTLE);
    break;

    case BATTLE_STATE_MOVE:
      if (init_battle_steps(battle, ba) == 0)
        return 0;
    break;

    case BATTLE_STATE_WALK:
    break;

    case BATTLE_STATE_ATTACK:
      if (init_battle_attack(battle, ba) == 0)
        return 0;
    break;

    case BATTLE_STATE_STRIKE:
      init_battle_strike(battle);
    break;

    case BATTLE_STATE_SELECT_MAGIC:
      set_menu(MENU_MAGIC);
    break;

    case BATTLE_STATE_TARGET:
      if ((spell = get_player_spell(ba->player, (int) arg)) == NULL)
        return 0;
      ba->arg = spell;
      if (init_battle_target(battle, ba, is_spell_support(spell)) == 0)
        return 0;
    break;

    default:
      free_battle_steps(ba);
      free_battle_attack(ba);
      free_battle_target(ba);
    break;
  }

  ba->state = state;
  return 1;
}

int set_curr_battle_state(
  BATTLE *battle,
  BATTLE_STATE state,
  void *arg
  )
{
  return set_battle_state(battle, battle->currActor, state, arg);
}

BATTLE_ACTOR* create_battle_actor(
  BATTLE *battle,
  PLAYER *player
  )
{
  BATTLE_ACTOR *ba;

  ba = (BATTLE_ACTOR *) malloc( sizeof(BATTLE_ACTOR) );
  if (ba == NULL) {

    fprintf(stderr, "Error - Out of memory\n");
    exit(1);

  }

  memset(ba, 0, sizeof(BATTLE_ACTOR));

  ba->type = ACTOR_HERO;
  ba->index = battle->counter++;
  ba->i = (player->x + player->h->w / 2) / BMARKER_WIDTH;
  ba->j = (player->y + player->h->h / 2) / BMARKER_HEIGHT;
  listInit(&ba->stepList);
  listInit(&ba->walkList);
  listInit(&ba->attackList);
  listInit(&ba->targetList);
  ba->cursor.i = ba->i;
  ba->cursor.j = ba->j;
  ba->player = player;
  player->x = ba->i * BMARKER_WIDTH;
  player->y = ba->j * BMARKER_HEIGHT;

  set_battle_state(battle, ba, BATTLE_STATE_IDLE, NULL);

  return ba;
}

BATTLE_ACTOR* new_battle_actor(
  BATTLE *battle,
  int i,
  int j,
  ACTOR *actor,
  ACTOR_TYPE type
  )
{
  BATTLE_ACTOR *ba;

  ba = (BATTLE_ACTOR *) malloc( sizeof(BATTLE_ACTOR) );
  if (ba == NULL) {

    fprintf(stderr, "Error - Out of memory\n");
    exit(1);

  }

  memset(ba, 0, sizeof(BATTLE_ACTOR));

  ba->type = type;
  ba->index = battle->counter++;
  ba->i = i;
  ba->j = j;
  listInit(&ba->stepList);
  listInit(&ba->walkList);
  listInit(&ba->attackList);
  listInit(&ba->targetList);
  ba->cursor.i = i;
  ba->cursor.j = j;
  ba->player = new_player(i * BMARKER_WIDTH, j * BMARKER_HEIGHT, actor);

  if ((ba->player->spells[0] = load_spell_XML("fire.stat")) == NULL) {
    fprintf(stderr, "ERROR - Unable to load spell.\n");
    exit(1);
  }

  if ((ba->player->spells[1] = load_spell_XML("heal.stat")) == NULL) {
    fprintf(stderr, "ERROR - Unable to load spell.\n");
    exit(1);
  }

  set_battle_state(battle, ba, BATTLE_STATE_IDLE, NULL);

  return ba;
}

static void set_curr_battle_actor(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  battle->currActor = ba;
  battle->currActor->cursor.i = battle->currActor->i;
  battle->currActor->cursor.j = battle->currActor->j;
  move_world(battle->world, battle->currActor->player);
}

void init_battle_player(
  BATTLE *battle,
  PLAYER *player
  )
{
  BATTLE_ACTOR *ba;

  ba = create_battle_actor(battle, player);
  listAdd(&battle->battleList, &ba->listNode);
  set_curr_battle_actor(battle, ba);
  set_curr_battle_state(battle, BATTLE_STATE_WAIT, NULL);
}

void deinit_battle(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba;

  for (ba = (BATTLE_ACTOR *) LIST_HEAD(&battle->battleList);
       ba != NULL;
       ba = (BATTLE_ACTOR *) LIST_NEXT(&ba->listNode))  {
    set_battle_state(battle, ba, BATTLE_STATE_DONE, NULL);
    /* TODO: Free each battle actor */
  }

  /* TODO: Free all nodes */
  listInit(&battle->battleList);
}

void free_battle(
  BATTLE *battle
  )
{
  deinit_battle(battle);
  free_sprite(battle->bmarker);
  free(battle);
}

static void next_battle_turn(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba;

  for (ba = (BATTLE_ACTOR *) LIST_HEAD(&battle->battleList);
       ba != NULL;
       ba = (BATTLE_ACTOR *) LIST_NEXT(&ba->listNode) ) {

    set_battle_state(battle, ba, BATTLE_STATE_IDLE, NULL);

  }
}

STEP_POINT* check_battle_step(
  BATTLE_ACTOR *ba,
  int i,
  int j
  )
{
  STEP_POINT *p;

  for (p = (STEP_POINT *) LIST_HEAD(&ba->stepList);
       p != NULL;
       p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

    if (i == p->i && j == p->j)
      return p;

  }

  return NULL;
}

static void select_battle_steps(
  BATTLE *battle,
  int input
  )
{
  STEP_POINT *p, *q;

  if (battle->currActor == NULL)
    return;

  if (input & PRESS_DOWN) {

    p = check_battle_step(battle->currActor,
			  battle->currActor->cursor.i,
			  battle->currActor->cursor.j + 1);

    if (p != NULL) {

      battle->currActor->cursor.j++;
      q = copy_battle_step(p);
      listAdd(&battle->currActor->walkList, &q->listNode);

    }

  }

  else if (input & PRESS_RIGHT) {

    p = check_battle_step(battle->currActor,
			  battle->currActor->cursor.i + 1,
			  battle->currActor->cursor.j);

    if (p != NULL) {

      battle->currActor->cursor.i++;
      q = copy_battle_step(p);
      listAdd(&battle->currActor->walkList, &q->listNode);

    }

  }

  else if (input & PRESS_UP) {

    p = check_battle_step(battle->currActor,
			  battle->currActor->cursor.i,
			  battle->currActor->cursor.j - 1);

    if (p != NULL) {

      battle->currActor->cursor.j--;
      q = copy_battle_step(p);
      listAdd(&battle->currActor->walkList, &q->listNode);

    }

  }

  else if (input & PRESS_LEFT) {

    p = check_battle_step(battle->currActor,
			  battle->currActor->cursor.i - 1,
			  battle->currActor->cursor.j);

    if (p != NULL) {

      battle->currActor->cursor.i--;
      q = copy_battle_step(p);
      listAdd(&battle->currActor->walkList, &q->listNode);

    }

  }

  else if (input & PRESS_ENTER) {

#ifdef DEBUG
    printf("Current actor: %d\n", battle->currActor->index);

    for (p = (STEP_POINT *) LIST_HEAD(&battle->currActor->walkList);
         p != NULL;
         p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

      printf("Step: [%d, %d]\n", p->i, p->j);

    }
#endif

    set_curr_battle_state(battle, BATTLE_STATE_WALK, NULL);

  }

}

static STEP_POINT* find_battle_destination(
  BATTLE_ACTOR *ba,
  PLAYER_DIR dir,
  LIST *list,
  int reach
  )
{
  int i;
  STEP_POINT *p, *ret;

  ret = NULL;

  switch(dir) {

    case PLAYER_FACE_DOWN:
      for (i = 1; i < reach + 1; i++) {
      
        for (p = (STEP_POINT *) LIST_HEAD(list);
             p != NULL;
             p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

          if (p->j == ba->cursor.j + i) {

            if (ret == NULL)
              ret = p;
            else if (abs(p->i - ba->cursor.i) < abs(ret->i - ba->cursor.i))
              ret = p;

          }

        }


      }
    break;

    case PLAYER_FACE_LEFT:
      for (i = 1; i < reach + 1; i++) {
      
        for (p = (STEP_POINT *) LIST_HEAD(list);
             p != NULL;
             p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

          if (p->i == ba->cursor.i - i) {

            if (ret == NULL)
              ret = p;
            else if (abs(p->j - ba->cursor.j) < abs(ret->j - ba->cursor.j))
              ret = p;

          }

        }


      }
    break;

    case PLAYER_FACE_RIGHT:
      for (i = 1; i < reach + 1; i++) {
      
        for (p = (STEP_POINT *) LIST_HEAD(list);
             p != NULL;
             p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

          if (p->i == ba->cursor.i + i) {

            if (ret == NULL)
              ret = p;
            else if (abs(p->j - ba->cursor.j) < abs(ret->j - ba->cursor.j))
              ret = p;

          }

        }

      }
    break;

    case PLAYER_FACE_UP:
      for (i = 1; i < reach + 1; i++) {
      
        for (p = (STEP_POINT *) LIST_HEAD(list);
             p != NULL;
             p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

          if (p->j == ba->cursor.j - i) {

            if (ret == NULL)
              ret = p;
            else if (abs(p->i - ba->cursor.i) < abs(ret->i - ba->cursor.i))
              ret = p;

          }

        }


      }
    break;

    default:
    break;
  }

  return ret;
}

static void select_battle_destination(
  BATTLE *battle,
  int input,
  LIST *list,
  int reach,
  void (*cb) (BATTLE*)
  )
{
  STEP_POINT *p;

  if (input & PRESS_DOWN) {

    p = find_battle_destination(battle->currActor, PLAYER_FACE_DOWN, list, reach);
    if (p != NULL) {

      battle->currActor->cursor.i = p->i;
      battle->currActor->cursor.j = p->j;

    }

  }

  else if (input & PRESS_LEFT) {

    p = find_battle_destination(battle->currActor, PLAYER_FACE_LEFT, list, reach);
    if (p != NULL) {

      battle->currActor->cursor.i = p->i;
      battle->currActor->cursor.j = p->j;

    }

  }

  else if (input & PRESS_RIGHT) {

    p = find_battle_destination(battle->currActor, PLAYER_FACE_RIGHT, list, reach);
    if (p != NULL) {

      battle->currActor->cursor.i = p->i;
      battle->currActor->cursor.j = p->j;

    }

  }

  else if (input & PRESS_UP) {

    p = find_battle_destination(battle->currActor, PLAYER_FACE_UP, list, reach);
    if (p != NULL) {

      battle->currActor->cursor.i = p->i;
      battle->currActor->cursor.j = p->j;

    }

  }

  else if (input & PRESS_ENTER) {

    cb(battle);

  }
}

static void attack_cb(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba;

  ba = get_battle_actor_at(battle,
                           battle->currActor,
                           battle->currActor->cursor.i,
                           battle->currActor->cursor.j,
                           0);
  if (ba == NULL) {

    fprintf(stderr, "ERROR - Invalid cursor position.\n");
    exit(1);
  }

#ifdef DEBUG
  printf("Attack character with index: %d\n",
         ba->index);
#endif

  set_curr_battle_state(battle, BATTLE_STATE_STRIKE, ba);
}

static void target_cb(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba;

  ba = get_battle_actor_at(battle,
                           battle->currActor,
                           battle->currActor->cursor.i,
                           battle->currActor->cursor.j,
                           1);
  if (ba == NULL) {

    fprintf(stderr, "ERROR - Invalid cursor position.\n");
    exit(1);
  }

#ifdef DEBUG
  printf("Target character with index: %d\n",
         ba->index);
#endif

  set_curr_battle_state(battle, BATTLE_STATE_DONE, ba);
}

void* get_curr_battle_spell(
  BATTLE *battle,
  int index
  )
{
  return get_player_spell(battle->currActor->player, index);
}

void move_battle_keydown(
  BATTLE *battle,
  int key
  )
{
  switch(battle->currActor->state) {

    case BATTLE_STATE_WAIT:
      move_menu_keydown(key);
    break;

    case BATTLE_STATE_MOVE:
      select_battle_steps(battle, get_input_keydown(key));
    break;

    case BATTLE_STATE_ATTACK:
      select_battle_destination(battle,
                                get_input_keydown(key),
                                &battle->currActor->attackList,
                                battle->currActor->player->h->status.reach,
                                attack_cb);
    break;

    case BATTLE_STATE_SELECT_MAGIC:
      move_menu_keydown(key);
    break;

    case BATTLE_STATE_TARGET:
      select_battle_destination(battle,
                                get_input_keydown(key),
                                &battle->currActor->targetList,
                                ((SPELL *) battle->currActor->arg)->status.reach,
                                target_cb);
    break;

    default:
    break;

  }

}

static int move_battle_walk(
  BATTLE_ACTOR *ba
  )
{
  STEP_POINT *p, *q;

  p = (STEP_POINT *) LIST_HEAD(&ba->walkList);
  if (p == NULL) {

    return 1;

  }

  q = (STEP_POINT *) LIST_NEXT(&p->listNode);
  if (q == NULL) {

    ba->i = p->i;
    ba->j = p->j;

#ifdef DEBUG
    printf("Last element [%d, %d]",
            p->i, p->j);
#endif

    listRemove(&ba->walkList, &p->listNode);
    free(p);

#ifdef DEBUG
    printf(", %d element(s) left\n",
           listCount(&ba->walkList));
#endif

    return 1;

  }

  if (move_player_to(ba->player,
                     q->i * BMARKER_WIDTH,
                     q->j * BMARKER_HEIGHT)) {

#ifdef DEBUG
    printf("[%d, %d] -> [%d, %d]",
           p->i, p->j,
           q->i, q->j);
#endif

    listRemove(&ba->walkList, &p->listNode);
    free(p);

#ifdef DEBUG
    printf(", %d element(s) left\n",
           listCount(&ba->walkList));
#endif

  }

  return 0;
}

static int move_battle_strike(
  BATTLE *battle
  )
{
  int result;
  int done = 0;
  BATTLE_ACTOR *ba = (BATTLE_ACTOR *) battle->currActor->arg;

  result = move_strike(battle->strike);
  if (result > 0) {

    ba->player->h->status.hp -= result;
    if (ba->player->h->status.hp <= 0) {

      ba->player->h->status.alive = 0;

#ifdef DEBUG
      printf("Character with index: %d perished.\n",
             ba->index);
#endif

      listRemove(&battle->battleList, &ba->listNode);
      /* TODO: Free character */
    }
    done = 1;
  }
  
  return done;
}

void move_battle(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba;

  switch(battle->currActor->state) {

    case BATTLE_STATE_IDLE:
      set_curr_battle_state(battle, BATTLE_STATE_WAIT, NULL);
    break;

    case BATTLE_STATE_WALK:

      if (move_battle_walk(battle->currActor)) {

        set_curr_battle_state(battle, BATTLE_STATE_DONE, NULL);

      }

    break;

    case BATTLE_STATE_STRIKE:
      if (move_battle_strike(battle)) {

        set_curr_battle_state(battle, BATTLE_STATE_DONE, NULL);

      }

    break;

    case BATTLE_STATE_DONE:

      do {

        for (ba = (BATTLE_ACTOR *) LIST_HEAD(&battle->battleList);
             ba != NULL;
             ba = (BATTLE_ACTOR *) LIST_NEXT(&ba->listNode) ) {

          if (ba->state == BATTLE_STATE_IDLE) {

#ifdef DEBUG
            printf("Change to actor: %d\n", ba->index);
#endif

            set_curr_battle_actor(battle, ba);
            return;

          }

        }

#ifdef DEBUG
        printf("Next turn\n");
#endif

        next_battle_turn(battle);

    } while(1);

    break;

    default:
    break;

  }

}

static void draw_battle_steps(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  int x, y;
  STEP_POINT *p;

  for (p = (STEP_POINT *) LIST_HEAD(&ba->stepList);
       p != NULL;
       p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

    x = p->i * BMARKER_WIDTH - battle->world->x;
    y = p->j * BMARKER_HEIGHT - battle->world->y;

    draw_sprite(x, y,
	        STEP_MARKER, battle->bmarker,
	        0, 0, window_width, window_height);

  }
}

static void draw_battle_walk(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  int x, y;
  STEP_POINT *p;

  for (p = (STEP_POINT *) LIST_HEAD(&ba->walkList);
       p != NULL;
       p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

    x = p->i * BMARKER_WIDTH - battle->world->x;
    y = p->j * BMARKER_HEIGHT - battle->world->y;

    draw_sprite(x, y,
	        WALK_MARKER, battle->bmarker,
	        0, 0, window_width, window_height);

  }
}

static void draw_battle_attack(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  int x, y;
  STEP_POINT *p;

  for (p = (STEP_POINT *) LIST_HEAD(&ba->attackList);
       p != NULL;
       p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

    x = p->i * BMARKER_WIDTH - battle->world->x;
    y = p->j * BMARKER_HEIGHT - battle->world->y;

    draw_sprite(x, y,
	        ATTACK_MARKER, battle->bmarker,
	        0, 0, window_width, window_height);

  }
}

char* list_curr_battle_spells(
  BATTLE *battle,
  char *str
  )
{
  list_player_spells(battle->currActor->player, str);

  return str;
}

static void draw_battle_target(
  BATTLE *battle,
  BATTLE_ACTOR *ba
  )
{
  int x, y;
  STEP_POINT *p;

  for (p = (STEP_POINT *) LIST_HEAD(&ba->targetList);
       p != NULL;
       p = (STEP_POINT *) LIST_NEXT(&p->listNode)) {

    x = p->i * BMARKER_WIDTH - battle->world->x;
    y = p->j * BMARKER_HEIGHT - battle->world->y;

    draw_sprite(x, y,
	        TARGET_MARKER, battle->bmarker,
	        0, 0, window_width, window_height);

  }
}

static void draw_battle_field(
  BATTLE *battle
  )
{
  BATTLE_ACTOR *ba;

  for (ba = (BATTLE_ACTOR *) LIST_HEAD(&battle->battleList);
       ba != NULL;
       ba = (BATTLE_ACTOR *) LIST_NEXT(&ba->listNode) ) {

    draw_player(ba->player, battle->world);

  }
}

void draw_battle(
  BATTLE *battle
  )
{
  switch(battle->currActor->state) {

    case BATTLE_STATE_WAIT:
      draw_battle_field(battle);
      draw_sprite(battle->currActor->cursor.i * BMARKER_WIDTH - battle->world->x,
                  battle->currActor->cursor.j * BMARKER_HEIGHT - battle->world->y,
	          CURR_CURSOR_MARKER, battle->bmarker,
	          0, 0, window_width, window_height);
      draw_menu(10, 10);
    break;

    case BATTLE_STATE_MOVE:
      draw_battle_steps(battle, battle->currActor);
      draw_battle_walk(battle, battle->currActor);
      draw_battle_field(battle);
      draw_sprite(battle->currActor->cursor.i * BMARKER_WIDTH - battle->world->x,
                  battle->currActor->cursor.j * BMARKER_HEIGHT - battle->world->y,
	          WALK_CURSOR_MARKER, battle->bmarker,
	          0, 0, window_width, window_height);
    break;

    case BATTLE_STATE_ATTACK:
      draw_battle_attack(battle, battle->currActor);
      draw_battle_field(battle);
      draw_sprite(battle->currActor->cursor.i * BMARKER_WIDTH - battle->world->x,
                  battle->currActor->cursor.j * BMARKER_HEIGHT - battle->world->y,
	          TARGET_CURSOR_MARKER, battle->bmarker,
	          0, 0, window_width, window_height);
    break;

    case BATTLE_STATE_STRIKE:
      draw_battle_field(battle);
      draw_strike(battle->strike);
    break;

    case BATTLE_STATE_SELECT_MAGIC:
      draw_battle_field(battle);
      draw_sprite(battle->currActor->cursor.i * BMARKER_WIDTH - battle->world->x,
                  battle->currActor->cursor.j * BMARKER_HEIGHT - battle->world->y,
	          CURR_CURSOR_MARKER, battle->bmarker,
	          0, 0, window_width, window_height);
      draw_menu(10, 10);
    break;

    case BATTLE_STATE_TARGET:
      draw_battle_target(battle, battle->currActor);
      draw_battle_field(battle);
      draw_sprite(battle->currActor->cursor.i * BMARKER_WIDTH - battle->world->x,
                  battle->currActor->cursor.j * BMARKER_HEIGHT - battle->world->y,
	          TARGET_CURSOR_MARKER, battle->bmarker,
	          0, 0, window_width, window_height);
    break;

    default:
      draw_battle_field(battle);
    break;


  }

}

