#ifndef IO_H
# define IO_H

class dungeon;
#include "dungeon.h"

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_display_no_fog(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
int io_handle_teleport(dungeon *d);
int io_teleport_pc(dungeon *d, pair_t p);
uint32_t io_teleport_pc_r(dungeon *d);

#endif
