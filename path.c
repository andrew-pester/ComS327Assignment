#include <stdlib.h>

#include "path.h"
#include "dungeon.h"
#include "utils.h"

/* Ugly hack: There is no way to pass a pointer to the dungeon into the *
 * heap's comparitor funtion without modifying the heap.  Copying the   *
 * pc_distance array is a possible solution, but that doubles the       *
 * bandwidth requirements for dijkstra, which would also be bad.        *
 * Instead, make a global pointer to the dungeon in this file,          *
 * initialize it in dijkstra, and use it in the comparitor to get to    *
 * pc_distance.  Otherwise, pretend it doesn't exist, because it really *
 * is ugly.                                                             */
static dungeon_t *dungeon;

typedef struct path
{
  heap_node_t *hn;
  uint8_t pos[2];
} path_t;

static int32_t dist_cmp(const void *key, const void *with)
{
  return ((int32_t)dungeon->pc_distance[((path_t *)key)->pos[dim_y]]
                                       [((path_t *)key)->pos[dim_x]] -
          (int32_t)dungeon->pc_distance[((path_t *)with)->pos[dim_y]]
                                       [((path_t *)with)->pos[dim_x]]);
}

static int32_t tunnel_cmp(const void *key, const void *with)
{
  return ((int32_t)dungeon->pc_tunnel[((path_t *)key)->pos[dim_y]]
                                     [((path_t *)key)->pos[dim_x]] -
          (int32_t)dungeon->pc_tunnel[((path_t *)with)->pos[dim_y]]
                                     [((path_t *)with)->pos[dim_x]]);
}

void dijkstra(dungeon_t *d)
{
  /* Currently assumes that monsters only move on floors.  Will *
   * need to be modified for tunneling and pass-wall monsters.  */

  heap_t h;
  uint32_t x, y;
  static path_t p[DUNGEON_Y][DUNGEON_X], *c;
  static uint32_t initialized = 0;

  if (!initialized)
  {
    initialized = 1;
    dungeon = d;
    for (y = 0; y < DUNGEON_Y; y++)
    {
      for (x = 0; x < DUNGEON_X; x++)
      {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      d->pc_distance[y][x] = 255;
    }
  }
  d->pc_distance[d->pc.position[dim_y]][d->pc.position[dim_x]] = 0;

  heap_init(&h, dist_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      if (mapxy(x, y) >= ter_floor)
      {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      }
    }
  }

  while ((c = heap_remove_min(&h)))
  {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
        (d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x]] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x]] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
        (d->pc_distance[c->pos[dim_y]][c->pos[dim_x] - 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y]][c->pos[dim_x] - 1] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
        (d->pc_distance[c->pos[dim_y]][c->pos[dim_x] + 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y]][c->pos[dim_x] + 1] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
        (d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x]] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x]] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1))
    {
      d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
          d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}

/* Ignores the case of hardness == 255, because if *
 * that gets here, there's already been an error.  */
#define tunnel_movement_cost(x, y) \
  ((d->hardness[y][x] / 85) + 1)

void dijkstra_tunnel(dungeon_t *d)
{
  /* Currently assumes that monsters only move on floors.  Will *
   * need to be modified for tunneling and pass-wall monsters.  */

  heap_t h;
  uint32_t x, y;
  int size;
  static path_t p[DUNGEON_Y][DUNGEON_X], *c;
  static uint32_t initialized = 0;

  if (!initialized)
  {
    initialized = 1;
    dungeon = d;
    for (y = 0; y < DUNGEON_Y; y++)
    {
      for (x = 0; x < DUNGEON_X; x++)
      {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      d->pc_tunnel[y][x] = 255;
    }
  }
  d->pc_tunnel[d->pc.position[dim_y]][d->pc.position[dim_x]] = 0;

  heap_init(&h, tunnel_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      if (mapxy(x, y) != ter_wall_immutable)
      {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      }
    }
  }

  size = h.size;
  while ((c = heap_remove_min(&h)))
  {
    if (--size != h.size)
    {
      exit(1);
    }
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
        (d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x]] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x]] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x] - 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x] - 1] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x] + 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x] + 1] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
        (d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x]] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x]] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
             tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y])))
    {
      d->pc_tunnel[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
          (d->pc_tunnel[c->pos[dim_y]][c->pos[dim_x]] +
           tunnel_movement_cost(c->pos[dim_x], c->pos[dim_y]));
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}

void straight_line(dungeon_t *d, monster_t *m)
{
  if (m->pos[dim_x] < d->pc.position[dim_x])
  {
    m->next_pos[dim_y] = m->pos[dim_y];
    m->next_pos[dim_x] = m->pos[dim_x] + 1;
  }
  else if (m->pos[dim_x] > d->pc.position[dim_x])
  {
    m->next_pos[dim_y] = m->pos[dim_y];
    m->next_pos[dim_x] = m->pos[dim_x] - 1;
  }
  //if y is equal wont go in either
  if (m->pos[dim_y] < d->pc.position[dim_y])
  {
    m->next_pos[dim_y] = m->next_pos[dim_y] + 1;
  }
  else if (m->pos[dim_y] > d->pc.position[dim_y])
  {
    m->next_pos[dim_y] = m->next_pos[dim_y] - 1;
  }
}
void shortest_path(dungeon_t *d, monster_t *m, int t)
{
  //uses the maps made by dijkstra to move the monster
  int mx = m->pos[dim_x];
  int my = m->pos[dim_y];
  int x = 0;
  int y = 0;
  int p = INT32_MAX;
  if (t)
  {
    if (d->pc_tunnel[my - 1][mx] < p)
    {
      x = 1;
      p = d->pc_tunnel[my - 1][mx];
    }
    if (d->pc_tunnel[my][mx + 1] < p)
    {
      x = 1;
      p = d->pc_tunnel[my][mx + 1];
    }
    if (d->pc_tunnel[my + 1][mx] < p)
    {
      y = 1;
      p = d->pc_tunnel[my + 1][mx];
    }
    if (d->pc_tunnel[my][mx - 1] < p)
    {
      x = -1;
      p = d->pc_tunnel[my][mx - 1];
    }
  }
  else
  {
    if (d->pc_distance[my - 1][mx] < p)
    {
      x = 1;
      p = d->pc_distance[my - 1][mx];
    }
    if (d->pc_distance[my][mx + 1] < p)
    {
      x = 1;
      p = d->pc_distance[my][mx + 1];
    }
    if (d->pc_distance[my + 1][mx] < p)
    {
      y = 1;
      p = d->pc_distance[my + 1][mx];
    }
    if (d->pc_distance[my][mx - 1] < p)
    {
      x = -1;
      p = d->pc_distance[my][mx - 1];
    }
  }
  m->next_pos[dim_x] = m->pos[dim_x] + x;
  m->next_pos[dim_y] = m->pos[dim_y] + y;
}

int32_t move_cmp(const void *key, const void *with)
{
  return ((monster_t *)key)->turn - ((monster_t *)with)->turn;
}
void move(dungeon_t *d, heap_t *h, monster_t *m)
{
  //checks the type of the monster and moves according to their protocol
  int erratic = rand() % 2;
  int same_room = in_room(d, d->pc.position[dim_y], d->pc.position[dim_x], m->pos[dim_y], m->pos[dim_x]);
  if (m->type == '0')
  {
    //none
    if (same_room)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == '1')
  {
    //intelligent
    if (same_room)
    {

      shortest_path(d, m, 0);
      m->last_known[dim_y] = d->pc.position[dim_y];
      m->last_known[dim_x] = d->pc.position[dim_x];
    }
    else if (m->last_known[dim_x] != -1 && m->last_known[dim_y] != -1)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == '2')
  {
    //telepathic
    straight_line(d, m);
  }
  else if (m->type == '3')
  {
    //intelligent
    //telepathic
    shortest_path(d, m, 0);
  }
  else if (m->type == '4')
  {
    //tunneling
    if (same_room)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == '5')
  {
    //tunneling
    //intelligent
    if (same_room)
    {
      shortest_path(d, m, 1);
      m->last_known[dim_y] = d->pc.position[dim_y];
      m->last_known[dim_x] = d->pc.position[dim_x];
    }
    else if (m->last_known[dim_x] != -1 && m->last_known[dim_y] != -1)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == '6')
  {
    //telepathic
    //tunneling
    straight_line(d, m);
  }
  else if (m->type == '7')
  {
    //tunneling
    //intelligent
    //telepathic
    shortest_path(d, m, 1);
  }
  else if (m->type == '8')
  {
    //erratic
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == '9')
  {
    //erratic
    //intelligent
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else if (same_room)
    {
      shortest_path(d, m, 0);
      m->last_known[dim_y] = d->pc.position[dim_y];
      m->last_known[dim_x] = d->pc.position[dim_x];
    }
    else if (m->last_known[dim_x] != -1 && m->last_known[dim_y] != -1)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == 'a')
  {
    //erratic
    //telepathic
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else
    {
      straight_line(d, m);
    }
  }
  else if (m->type == 'b')
  {
    //erratic
    //intelligent
    //telepathic
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else
    {
      shortest_path(d, m, 0);
    }
  }
  else if (m->type == 'c')
  {
    //tunneling
    //erratic
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else if (same_room)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == 'd')
  {
    //tunneling
    //erratic
    //intelligent
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else if (same_room)
    {
      shortest_path(d, m, 1);
      m->last_known[dim_y] = d->pc.position[dim_y];
      m->last_known[dim_x] = d->pc.position[dim_x];
    }
    else if (m->last_known[dim_x] != -1 && m->last_known[dim_y] != -1)
    {
      straight_line(d, m);
    }
    else
    {
      m->next_pos[dim_x] = m->pos[dim_x];
      m->next_pos[dim_y] = m->pos[dim_y];
    }
  }
  else if (m->type == 'e')
  {
    //tunneling
    //erratic
    //telepathic
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else
    {
      straight_line(d, m);
    }
  }
  else if (m->type == 'f')
  {
    //tunneling
    //erratic
    //intelligent
    //telepathic
    if (erratic)
    {
      int x = rand_range(-1, 1);
      int y = rand_range(-1, 1);
      m->next_pos[dim_x] = m->pos[dim_x] + x;
      m->next_pos[dim_y] = m->pos[dim_y] + y;
    }
    else
    {
      shortest_path(d, m, 1);
    }
  }
  else if (m->type == '@')
  {
    //pc moves randomly
    int x = rand_range(-1, 1);
    int y = rand_range(-1, 1);
    m->next_pos[dim_x] = m->pos[dim_x] + x;
    m->next_pos[dim_y] = m->pos[dim_y] + y;
  }
  else
  {
    //if it is not  type that is recognized then go to next monster found
    //done as safeguard
    return;
  }

  //if they aren't tunneling and they find a hardness !=0 i.e. a wall then they don't move
  //catches illegal moves from straight_line
  if ((m->type == '0' || m->type == '1' || m->type == '2' || m->type == '3' || m->type == '8' || m->type == '9' || m->type == 'a' || m->type == 'b' || m->type == '@') && d->hardness[m->next_pos[dim_y]][m->next_pos[dim_x]] != 0)
  {
    m->next_pos[dim_x] = m->pos[dim_x];
    m->next_pos[dim_y] = m->pos[dim_y];
    m->turn = m->turn + 1000 / m->speed;
    m->hn = heap_insert(h, m);
  }
  else
  {
    //they are either tunneling or it is a valid move
    m->turn = m->turn + 1000 / m->speed;
    m->hn = heap_insert(h, m);
  }
}