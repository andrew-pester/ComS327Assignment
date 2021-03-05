#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "dungeon.h"
#include "path.h"

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <number of monsters>]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  uint32_t nummon = 0;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof(d));

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */

  if (argc > 1)
  {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0)
    {
      if (argv[i][0] == '-')
      { /* All switches start with a dash */
        if (argv[i][1] == '-')
        {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1])
        {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */)
          {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load")))
          {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save")))
          {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
            if (!strcmp(argv[++i], "seed"))
            {
              do_save_seed = 1;
              do_save_image = 0;
            }
            else if (!strcmp(argv[i], "image"))
            {
              do_save_image = 1;
              do_save_seed = 0;
            }
            else
            {
              save_file = argv[i];
            }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image")))
          {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;

        case 'n':
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            nummon = atoi(argv[++i]);
          }
          break;
        default:
          usage(argv[0]);
        }
      }
      else
      { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed)
  {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);
  if (nummon == 0)
  {
    nummon = 10;
  }

  if (do_load)
  {
    read_dungeon(&d, load_file);
  }
  else if (do_image)
  {
    read_pgm(&d, pgm_file);
  }
  else
  {
    gen_dungeon(&d, nummon);
  }

  if (!do_load)
  {
    /* Set a valid position for the PC */
    d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  //initalized pc in monster grid to keep track of easily and make moving easier
  d.monster[d.pc.position[dim_y]][d.pc.position[dim_x]].type = '@';
  d.monster[d.pc.position[dim_y]][d.pc.position[dim_x]].pos[dim_x] = d.pc.position[dim_x];
  d.monster[d.pc.position[dim_y]][d.pc.position[dim_x]].pos[dim_y] = d.pc.position[dim_y];
  d.monster[d.pc.position[dim_y]][d.pc.position[dim_x]].speed = 10;
  d.monster[d.pc.position[dim_y]][d.pc.position[dim_x]].id = nummon;
  dijkstra(&d);
  dijkstra_tunnel(&d);
  render_dungeon(&d);

  heap_t h;
  heap_init(&h, move_cmp, NULL);
  //initalizes the heap with the monsters
  for (i = 0; i < DUNGEON_Y; i++)
  {
    for (int j = 0; j < DUNGEON_X; j++)
    {
      if (d.monster[i][j].type != ' ')
      {
        //move if it is a monster else set hn to null
        move(&d, &h, &d.monster[i][j]);
      }
      else
      {
        d.monster[i][j].hn = NULL;
      }
    }
  }
  int running = 1;
  monster_t *m;
  //doesn't run this
  while (running && (m = heap_remove_min(&h)))
  {
    //this if doesn't work
    //there is no monster here so remove it from the heap
    while (m->id == -1)
    {
      m->hn = NULL;
      m = heap_remove_min(&h);
    }
    //sometimes runs multiple times  
    if (d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].type != ' ')
    {
      if (m->next_pos[dim_y] == d.pc.position[dim_y] && m->next_pos[dim_x] == d.pc.position[dim_x] && m->type != '@')
      {
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nGAME OVER!\n");
        running = 0;
      }
      else
      {
        //monster got hit by other monster
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]] = *m;
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].pos[dim_x] = m->next_pos[dim_x];
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].pos[dim_y] = m->next_pos[dim_y];
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].hn = NULL;
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].type = ' ';
      }
    }
    else if (d.hardness[m->next_pos[dim_y]][m->next_pos[dim_x]] != 0)
    {
      if (d.hardness[m->next_pos[dim_y]][m->next_pos[dim_x]] <= 85)
      {
        d.hardness[m->next_pos[dim_y]][m->next_pos[dim_x]] = 0;
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]] = *m;
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].pos[dim_x] = m->next_pos[dim_x];
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].pos[dim_y] = m->next_pos[dim_y];
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].hn = NULL;
      }
      else
      {
        d.hardness[m->next_pos[dim_y]][m->next_pos[dim_x]] = d.hardness[m->next_pos[dim_y]][m->next_pos[dim_x]] - 85;
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].next_pos[dim_x] = d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].next_pos[dim_x];
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].next_pos[dim_y] = d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].next_pos[dim_y];
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]] = *m;
        d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].hn = NULL;
        
      }

      if (d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].type == '@')
      {
        d.pc.position[dim_y] = d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].next_pos[dim_y];
        d.pc.position[dim_x] = d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]].next_pos[dim_x];

        dijkstra_tunnel(&d);
        dijkstra(&d);

        render_dungeon(&d);

        usleep(250000);
      }
      move(&d, &h, &d.monster[m->next_pos[dim_y]][m->next_pos[dim_x]]);
    }
    else
    {
      //printf("error\n");
    }
  }

  if (do_save)
  {
    if (do_save_seed)
    {
      /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image)
    {
      if (!pgm_file)
      {
        fprintf(stderr, "No image file was loaded.  Using default.\n");
        do_save_image = 0;
      }
      else
      {
        /* Extension of 3 characters longer than image extension + null. */
        save_file = malloc(strlen(pgm_file) + 4);
        strcpy(save_file, pgm_file);
        strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image)
    {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}
