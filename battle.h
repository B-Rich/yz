#ifndef _battle_h
#define _battle_h

#include "list.h"

#define BMARKER_WIDTH	       32
#define BMARKER_HEIGHT	       32
#define DEFAULT_RANGE           4
#define STEP_MARKER             0
#define WALK_MARKER             1
#define ATTACK_MARKER           2
#define TARGET_MARKER           3
#define CURR_CURSOR_MARKER      4
#define WALK_CURSOR_MARKER      5
#define TARGET_CURSOR_MARKER    6

typedef struct {
  LIST_NODE listNode;
  int i, j;
} STEP_POINT;

typedef struct {
  LIST_NODE listNode;
  ACTOR_TYPE type;
  int index;
  int i, j;
  LIST stepList;
  LIST walkList;
  LIST attackList;
  LIST targetList;
  STEP_POINT cursor;
  void *arg;
  BATTLE_STATE state;
  PLAYER *player;
} BATTLE_ACTOR;

typedef struct {
  int counter;
  LIST battleList;
  WORLD *world;
  BATTLE_ACTOR *currActor;
  SPRITE *bmarker;
  STRIKE *strike;
} BATTLE;

BATTLE* new_battle(
  WORLD *world
  );

int init_battle(
  BATTLE *battle,
  WORLD *world
  );

BATTLE_ACTOR* get_battle_actor_at(
  BATTLE *battle,
  BATTLE_ACTOR *curr,
  int i,
  int j,
  int self
  );

int set_curr_battle_state(
  BATTLE *battle,
  BATTLE_STATE state,
  void *arg
  );

BATTLE_ACTOR* create_battle_actor(
  BATTLE *battle,
  PLAYER *player
  );

BATTLE_ACTOR* new_battle_actor(
  BATTLE *battle,
  int i,
  int j,
  ACTOR *actor,
  ACTOR_TYPE type
  );

STEP_POINT* check_battle_step(
  BATTLE_ACTOR *ba,
  int i,
  int j
  );

void init_battle_player(
  BATTLE *battle,
  PLAYER *player
  );

void deinit_battle(
  BATTLE *battle
  );

void free_battle(
  BATTLE *battle
  );

void* get_curr_battle_spell(
  BATTLE *battle,
  int index
  );

void move_battle_keydown(
  BATTLE *battle,
  int key
  );

void move_battle(
  BATTLE *battle
  );

char* list_curr_battle_spells(
  BATTLE *battle,
  char *str
  );

void draw_battle(
  BATTLE *battle
  );

#endif /* _battle_h */

