#include <ncurses.h>
#include <stdlib.h>

#include "dungeon.h"
#include "move.h"
#include "pc.h"
#include "utils.h"
#include "path.h"
#include "io.h"
#include "npc.h"
#include "character.h"

void io_init_terminal(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}
void render_dungeon(dungeon_t *d)
{
    pair_t p;

    clear();
    addch('\n');
    //need to clear scrren to print new dungeon
    for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++)
    {
        for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++)
        {
            if (charpair(p))
            {
                addch(charpair(p)->symbol);
            }
            else
            {
                switch (mappair(p))
                {
                case ter_wall:
                case ter_wall_immutable:
                    mvaddch(p[dim_y] + 1, p[dim_x], ' ');
                    break;
                case ter_floor:
                case ter_floor_room:
                    mvaddch(p[dim_y] + 1, p[dim_x], '.');
                    break;
                case ter_floor_hall:
                    mvaddch(p[dim_y] + 1, p[dim_x], '#');
                    break;
                case ter_debug:
                    mvaddch(p[dim_y] + 1, p[dim_x], '*');
                    break;
                case ter_stairs_up:
                    mvaddch(p[dim_y] + 1, p[dim_x], '<');
                    break;
                case ter_stairs_down:
                    mvaddch(p[dim_y] + 1, p[dim_x], '>');
                    break;
                default:
                    mvaddch(p[dim_y] + 1, p[dim_x], ' ');
                    break;
                }
            }
        }
        addch('\n');
    }
    refresh();
}
void io_input(dungeon_t *d, pair_t dir)
{
    int i = 1;

    do
    {
        i = 1;
        int key = getch();
        switch (key)
        {
        //ul
        case '7':
        case 'y':
        case KEY_HOME:
            dir[dim_x] = -1;
            dir[dim_y] = -1;
            break;
        //u
        case '8':
        case 'k':
        case KEY_UP:
            dir[dim_x] = 0;
            dir[dim_y] = -1;
            break;
        //ur
        case '9':
        case 'u':
        case KEY_PPAGE:
            dir[dim_x] = 1;
            dir[dim_y] = -1;
            break;
        //r
        case '6':
        case 'l':
        case KEY_RIGHT:
            dir[dim_x] = 1;
            dir[dim_y] = 0;
            break;
        //dr
        case '3':
        case 'n':
        case KEY_NPAGE:
            dir[dim_x] = 1;
            dir[dim_y] = 1;
            break;
        //d
        case '2':
        case 'j':
        case KEY_DOWN:
            dir[dim_x] = 0;
            dir[dim_y] = 1;
            break;
        //dl
        case '1':
        case 'b':
        case KEY_END:
            dir[dim_x] = -1;
            dir[dim_y] = 1;
            break;
        //l
        case '4':
        case 'h':
        case KEY_LEFT:
            dir[dim_x] = -1;
            dir[dim_y] = 0;
            break;
        //go downstairs
        case '>':
            if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_down)
            {
                delete_dungeon(d);
                pc_delete(d->pc.pc);
                init_dungeon(d);
                gen_dungeon(d);
                config_pc(d);
                gen_monsters(d);
                clear();
                render_dungeon(d);
                refresh();
            }
            i = 2;

        //go upstairs
        case '<':
            if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_up)
            {
                delete_dungeon(d);
                pc_delete(d->pc.pc);
                init_dungeon(d);
                gen_dungeon(d);
                config_pc(d);
                gen_monsters(d);
                clear();
                render_dungeon(d);
                refresh();
            }
            i = 2;
        //rest
        case '5':
        case ' ':
        case '.':
        case KEY_B2:
            dir[dim_x] = 0;
            dir[dim_y] = 0;
            break;
        //dispaly monster list
        case 'm':
            clear();
            gen_monster_list(d,0);
            //render monster list
            clear();
            render_dungeon(d);
            i = 1;
            break;
        case 'Q':
            //done
            clear();
            d->quit = 1;
            i = 0;
            break;
        default:
            break;
        }
        //check if the next area is a wall if so don't move
        pair_t tmp;
        tmp[dim_y] = d->pc.position[dim_y] + dir[dim_y];
        tmp[dim_x] = d->pc.position[dim_x] + dir[dim_x];
        if (!(hardnesspair(tmp) != 0) && i != 2)
        {
            i = 0;
        }
    } while (i);
}
void gen_monster_list(dungeon_t *d, int start)
{
    character_t **c;

    //gets list of monsters
    c = malloc(d->num_monsters * sizeof(*c));
    int count = 0;
    for (int i = 0; i < DUNGEON_Y; i++)
    {
        for (int j = 0; j < DUNGEON_X; j++)
        {
            //if it exists and is not the player
            if (d->character[i][j] && d->character[i][j]->pc != d->pc.pc)
            {
                c[count++] = d->character[i][j];
            }
        }
    }
    scroll_list(d, 0, count, c);
    free(c);
}
int scroll_list(dungeon_t *d, int start, int count, character_t ** c){
    int index = 1;

    for (int i = start; i < count && index< 23; i++)
    {
        mvaddch(index, 20, c[i]->symbol);
        mvprintw(index, 22, "This monster is ");

        if (c[i]->position[dim_x] - d->pc.position[dim_x] <= 0)
        {
            mvprintw(index, 38, "West");
            mvprintw(index, 43, "%d", d->pc.position[dim_x] - c[i]->position[dim_x]);
        }
        else if (c[i]->position[dim_x] - d->pc.position[dim_x] >= 0)
        {
            mvprintw(index, 38, "East");
            mvprintw(index, 43, "%d", c[i]->position[dim_x] - d->pc.position[dim_x]);
        }
        if (c[i]->position[dim_y] - d->pc.position[dim_y] <= 0)
        {
            mvprintw(index, 46, "North");
            mvprintw(index, 52, "%d", d->pc.position[dim_y] - c[i]->position[dim_y]);
        }
        else if (c[i]->position[dim_y] - d->pc.position[dim_y] >= 0)
        {
            mvprintw(index, 46, "South");
            mvprintw(index, 52, "%d", c[i]->position[dim_y] - d->pc.position[dim_y]);
        }

        index += 2;
    }
    while (1)
    {
        int key = getch();
        //esc is pressed and exit the monster menu and regain control
        switch(key)
        {   
            case 27:
            return 1;
            //scroll up in monster list
            case KEY_UP:
            clear();
            if(start>0){
                start--;
            }
            scroll_list(d, start, count, c);
            return 1;
            //scroll down in the monster list
            case KEY_DOWN:
            clear();
            if(start<d->num_monsters-1){
                start++;
            }
            scroll_list(d, start, count, c);
            return 1;
        }
    }
} 
