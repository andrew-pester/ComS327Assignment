#ifndef IO_H
# define IO_H

typedef struct dungeon dungeon_t;

void io_init_terminal(void);
void render_dungeon(dungeon_t *d);
void io_input(dungeon_t *d, pair_t dir);
void gen_monster_list(dungeon_t *d,int start);
int scroll_list(dungeon_t *d, int start, int count, character_t ** c);

#endif