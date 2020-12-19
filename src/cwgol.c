#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define DELAY 60000
#define HELPFILE "helpfile"
#define BUFF_SIZE 100

#define ICON "#"

/* Conditions in which cells can be in */
enum CONDITION {
  ALIVE,
  DEAD,
  RESURRECT,
  DYING
};

/* FUNCTION PROTOTYPES */
void update_condition(enum CONDITION cells[*][*], 
                        unsigned int x, unsigned int y); 

void initialize_board(enum CONDITION cells[*][*],
						unsigned odds_alive,
						unsigned seed);

/* GLOBAL VARIABLES */
unsigned max_x = 0, max_y = 0;
int main(int argc, char *argv[]) {
  int seed = time(0), odds = 80;
  int *arg_pointer = NULL;
  unsigned i, j;


  /* Loop through arguments */
  for (i = 1; i < argc; i++) {

	/* Print out help page */
	if(strcmp(argv[i], "-h") == 0 ||
			strcmp(argv[i], "--help") == 0) {
		FILE *hstream;
		char buff[BUFF_SIZE];
		hstream = fopen(HELPFILE, "r");
		if (hstream == NULL) {
			fprintf(stderr, "Error opening file: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		while(fgets(buff, BUFF_SIZE, hstream) != NULL) {
			printf(buff);
		}
		fclose(hstream);
		exit(EXIT_SUCCESS);
	}

	/* Point to seed */
	else if (strcmp(argv[i], "-s") == 0)
		arg_pointer = &seed;

	/* Point to odds */
	else if (strcmp(argv[i], "-p") == 0)
		arg_pointer = &odds;

	/* Load argument into program */
	else if (arg_pointer != NULL) {
		sscanf(argv[i], "%d", arg_pointer);
		arg_pointer = NULL;
	}	
	
	/* User inputted unrecognized input */	
	else {
		printf("Error: Unrecognized input %s, Quitting...\n", argv[i]);
		exit(EXIT_FAILURE);
	}
  }

  /* Set up ncurses */
  initscr();
  noecho();
  curs_set(FALSE);
  getmaxyx(stdscr, max_y, max_x);

  /* Create board */
  enum CONDITION cells[max_x][max_y];
  initialize_board(cells, odds, seed);
  
  /* Continue until CTRL+C */
  while (true) {
    
	/* Update conditions of cells */
	for(i = 0; i < max_x; i++) {
      for(j = 0; j < max_y; j++) {
        update_condition(cells, i, j);
      }
    }

	/* Clear the board */
    erase();
    for(i = 0; i < max_x; i++) {
      for(j = 0; j < max_y; j++) {

		/* Make dying cells dead */
        if(cells[i][j] == DYING)
          cells[i][j] = DEAD;

		/* Print alive cells */
		else if(cells[i][j] == ALIVE)
          mvprintw(j, i, ICON);
        
		/* Print cells that are being
		 * revived */
		else if(cells[i][j] == RESURRECT) {
          mvprintw(j, i, ICON);
          cells[i][j] = ALIVE;
        }
      }
    }
	/* Refresh the board */
    refresh();
    usleep(DELAY);
  }

  /* Clear the board (Technically this is never called) */
  endwin();
  exit(EXIT_SUCCESS);
}

/* Initializes all the cells on the board */
void initialize_board(enum CONDITION cells[max_x][max_y],
						unsigned odds_alive,
						unsigned seed) {
  int i, j;

  /* Set up random-ness */
  srand(seed);

  /* Spawn initial creatures */
  for (i = 0; i < max_x; i++) {
    for (j = 0; j < max_y; j++) {

	  /* Randomize based on seed */
      if ((rand() % 100) < odds_alive) {
        cells[i][j] = DEAD;
      } else {
        cells[i][j] == ALIVE;
      }
    }
  }
}

/* Updates the condition of each cell */
void update_condition(enum CONDITION cells[max_x][max_y], 
                        unsigned int x, unsigned int y) {
  unsigned short alive_count = 0;
  int i = 0, j = 0;
  
  /* Get count of neighbors */
  for(i = x-1; i <= x+1; i++) {
	for(j = y-1; j <= y+1; j++) {
		/* Check each corner */
		if ((i >= 0 && j >= 0)
				&& (i < max_x && j < max_y)
				&& !(i == x && j == y) 
				&& (cells[i][j] == ALIVE || cells[i][j] == DYING))
			alive_count++;
	}
  }
  
  /* Three live neighbors survives (if dead)*/
  if (cells[x][y] == DEAD && alive_count == 3)
    cells[x][y] = RESURRECT;
  
  /* Fewer than two neighbours or more than two die */
  else if (cells[x][y] == ALIVE && (alive_count < 2 || alive_count > 3))
    cells[x][y] = DYING;
}
