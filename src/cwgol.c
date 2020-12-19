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

#define abs(n) ((n < 0) ? -n : n)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define PI 3.14159265

#define DELAY 30000

#define ICON "#"

enum CONDITION {
  ALIVE,
  DEAD,
  RESURRECT,
  DYING
};

void get_condition(enum CONDITION cells[*][*], 
                        unsigned int x, unsigned int y); 

int max_x = 0, max_y = 0;
int main(int argc, char *argv[]) {
  initscr();
  noecho();
	curs_set(FALSE);

  getmaxyx(stdscr, max_y, max_x);

  char buff[50];
  enum CONDITION cells[max_x][max_y];
  int i, j;
  /* Set up random-ness */
  srand((unsigned) time(0));

  /* Spawn initial creatures */
  for (i = 0; i < max_x; i++) {
    for (j = 0; j < max_y; j++) {
      if ((rand() % 100) < 80) {
        cells[i][j] = DEAD;
      } else {
        cells[i][j] == ALIVE;
      }
    }
  }
  
  while (true) {
    erase();
    for(i = 0; i < max_x; i++) {
      for(j = 0; j < max_y; j++) {
        get_condition(cells, i, j);
      }
    }

    for(i = 0; i < max_x; i++) {
      for(j = 0; j < max_y; j++) {
        if(cells[i][j] == DYING)
          cells[i][j] = DEAD;

        if(cells[i][j] == ALIVE) {
          mvprintw(j, i, ICON);
        }
        
        if(cells[i][j] == RESURRECT) {
          mvprintw(j, i, ICON);
          cells[i][j] = ALIVE;
        }
      }
    }
    refresh();
    usleep(DELAY);
  }

  endwin();
}

void get_condition(enum CONDITION cells[max_x][max_y], 
                        unsigned int x, unsigned int y) {
  unsigned short alive_count = 0;

  /* Get count of neighbors */
  /* On the left */
  if (x > 0 && (cells[x-1][y] == ALIVE || cells[x-1][y] == DYING))
    alive_count++;

  /* On the right */
  if (x < max_x - 1 && (cells[x+1][y] == ALIVE || cells[x+1][y] == DYING))
    alive_count++;

  /* On top */
  if (y < 0 && (cells[x][y-1] == ALIVE || cells[x][y-1] == DYING))
    alive_count++;

  /* On bottom */
  if (y < max_y - 1 && (cells[x][y+1] == ALIVE || cells[x][y+1] == DYING))
    alive_count++;

  /* Top left */
  if ((y > 0 && x > 0) && (cells[x-1][y-1] == ALIVE || cells[x-1][y-1] == DYING))
    alive_count++;
  
  /* Bottom left */
  if ((y < max_y && x > 0) && (cells[x-1][y+1] == ALIVE || cells[x-1][y+1] == DYING))
    alive_count++;
  
  /* Top right */
  if ((y > 0 && x < max_x - 1) && (cells[x+1][y-1] == ALIVE || cells[x+1][y-1] == DYING))
    alive_count++;
  
  /* Bottom right */
  if ((cells[x+1][y+1] == ALIVE || cells[x+1][y+1] == DYING))
    alive_count++;

  /* Three live neighbors survives (if dead)*/
  if (alive_count == 3 && cells[x][y] == DEAD)
    cells[x][y] = RESURRECT;
  
  /* Fewer than two neighbours or more than two die */
  else if (cells[x][y] == ALIVE && (alive_count < 2 || alive_count > 3))
    cells[x][y] = DYING;
}
