#ifndef PATH_H
# define PATH_H

# define HARDNESS_PER_TURN 85

#include "heap.h"
#include "dungeon.h"

typedef struct dungeon dungeon_t;

void dijkstra(dungeon_t *d);
void dijkstra_tunnel(dungeon_t *d);
int32_t move_cmp(const void *key, const void *with);
void move(dungeon_t *d, heap_t *h, monster_t *monster);

#endif
