#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
#include "dungeon.h"

class pc : public character {
public:
    terrain_type_t visited[DUNGEON_Y][DUNGEON_X];
};
void pc_visited(dungeon *d);
void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, int32_t room);
void reset_visited(dungeon *d);
int character_within_range(character *c, dungeon *d);

#endif
