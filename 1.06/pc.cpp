#include <stdlib.h>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"

void pc_delete(pc *pc)
{
  if (pc) {
    delete pc;
  }
}

uint32_t pc_is_alive(dungeon *d)
{
  return d->player->alive;
}

void place_pc(dungeon *d)
{
  d->player->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->player->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon *d)
{
  d->player = new pc;
  memset(d->player, 0, sizeof (*d->player));
  d->player->symbol = '@';
  //emset(d->player->visited,0, sizeof(d->player->visited));
  place_pc(d);

  d->player->speed = PC_SPEED;
  d->player->alive = 1;
  d->player->sequence_number = 0;
  d->player->kills[kill_direct] = d->player->kills[kill_avenged] = 0;
  for(int i = 0; i<DUNGEON_Y;i++){
    for(int j = 0; j<DUNGEON_X;j++){
      d->player->visited[i][j] = ter_wall;
    }
  }
  pc_visited(d);

  d->characters[d->player->position[dim_y]][d->player->position[dim_x]] = d->player;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, d->player)) {
    if (!count) {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(d->player->position[dim_x] - 1, d->player->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  } else if (charxy(d->player->position[dim_x], d->player->position[dim_y] - 1)) {
    dir[dim_y] = -1;
  } else if (charxy(d->player->position[dim_x] + 1, d->player->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  } else if (charxy(d->player->position[dim_x] - 1, d->player->position[dim_y])) {
    dir[dim_x] = -1;
  } else if (charxy(d->player->position[dim_x] + 1, d->player->position[dim_y])) {
    dir[dim_x] = 1;
  } else if (charxy(d->player->position[dim_x] - 1, d->player->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  } else if (charxy(d->player->position[dim_x], d->player->position[dim_y] + 1)) {
    dir[dim_y] = 1;
  } else if (charxy(d->player->position[dim_x] + 1, d->player->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  } else if (!have_seen_corner || count < 250) {
    /* Head to a corner and let most of the Nplayers kill each other off */
    if (count) {
      count++;
    }
    if (!against_wall(d, d->player) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir_nearest_wall(d, d->player, dir);
    }
  }else {
    /* And after we've been there, let's head toward the center of the map. */
    if (!against_wall(d, d->player) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir[dim_x] = ((d->player->position[dim_x] > DUNGEON_X / 2) ? -1 : 1);
      dir[dim_y] = ((d->player->position[dim_y] > DUNGEON_Y / 2) ? -1 : 1);
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(d->player->position[dim_x] + dir[dim_x],
              d->player->position[dim_y] + dir[dim_y]) &&
      ((charxy(d->player->position[dim_x] + dir[dim_x] - 1,
               d->player->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->player->position[dim_x] + dir[dim_x] - 1,
                d->player->position[dim_y] + dir[dim_y] - 1) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x] - 1,
               d->player->position[dim_y] + dir[dim_y]) &&
        (charxy(d->player->position[dim_x] + dir[dim_x] - 1,
                d->player->position[dim_y] + dir[dim_y]) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x] - 1,
               d->player->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->player->position[dim_x] + dir[dim_x] - 1,
                d->player->position[dim_y] + dir[dim_y] + 1) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x],
               d->player->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->player->position[dim_x] + dir[dim_x],
                d->player->position[dim_y] + dir[dim_y] - 1) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x],
               d->player->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->player->position[dim_x] + dir[dim_x],
                d->player->position[dim_y] + dir[dim_y] + 1) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x] + 1,
               d->player->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->player->position[dim_x] + dir[dim_x] + 1,
                d->player->position[dim_y] + dir[dim_y] - 1) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x] + 1,
               d->player->position[dim_y] + dir[dim_y]) &&
        (charxy(d->player->position[dim_x] + dir[dim_x] + 1,
                d->player->position[dim_y] + dir[dim_y]) != d->player)) ||
       (charxy(d->player->position[dim_x] + dir[dim_x] + 1,
               d->player->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->player->position[dim_x] + dir[dim_x] + 1,
                d->player->position[dim_y] + dir[dim_y] + 1) != d->player)))) {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t pc_in_room(dungeon *d, int32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->player->position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->player->position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->player->position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->player->position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}
void pc_visited(dungeon *d){
  int xmin= 0, xmax=0, ymin=0, ymax=0;
  xmin = d->player->position[dim_x]-2;
  if(xmin<0){
    xmin = 0;
  }
  xmax = d->player->position[dim_x]+3;
  if(xmax>DUNGEON_X){
    xmax = DUNGEON_X;
  }
  ymin = d->player->position[dim_y]-2;
  if(ymin<0){
    ymin = 0;
  }
  ymax = d->player->position[dim_y]+3;
  if(ymax>DUNGEON_Y){
    ymax = DUNGEON_Y;
  }
  for(int i = ymin; i<ymax;i++){
    for(int j = xmin; j<xmax;j++){
      d->player->visited[i][j] = d->map[i][j];
    }
  }
}
void reset_visited(dungeon *d){
  for(int i = 0; i<DUNGEON_Y;i++){
    for(int j = 0; j<DUNGEON_X;j++){
      d->player->visited[i][j] = ter_wall;
    }
  }
}
int character_within_range(character *c, dungeon *d){
  int xmin= 0, xmax=0, ymin=0, ymax=0;
  xmin = d->player->position[dim_x]-2;
  if(xmin<0){
    xmin = 0;
  }
  xmax = d->player->position[dim_x]+3;
  if(xmax>DUNGEON_X){
    xmax = DUNGEON_X;
  }
  ymin = d->player->position[dim_y]-2;
  if(ymin<0){
    ymin = 0;
  }
  ymax = d->player->position[dim_y]+3;
  if(ymax>DUNGEON_Y){
    ymax = DUNGEON_Y;
  }
  for(int i = ymin; i<ymax;i++){
    for(int j = xmin; j<xmax;j++){
      if(c->position[dim_x] == j  && c->position[dim_y] == i){
        return 1;
      }
    }
  }
  return 0;
}
