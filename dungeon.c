#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
char dung[21][80];
struct room
{
  int x;      //x coord in array
  int y;      //y coord in array
  int x_size; //random x size
  int y_size; //random y size
};
int main(int argc, char *argv[])
{
  srand(time(NULL));
  struct room rooms[6];
  for (int i = 0; i < 21; i++)
  {
    for (int j = 0; j < 80; j++)
    {
      dung[i][j] = ' ';
    }
  }
  makeRooms(rooms);

  for (int i = 0; i < 21; i++)
  {
    for (int j = 0; j < 80; j++)
    {
      printf("%c", dung[i][j]);
    }
    printf("\n");
  }
}
int makeRooms(struct room *rooms)
{
  //makes rooms in 6 different localized areas so the generation is random but rooms will never hit
  //this works for generating random rooms
  int tmpX = 0, tmpY = 0;
  int up = rand() % 6;   //random room from 0 - 5
  int down = rand() % 6; //random room from 0 - 5
  for (int i = 0; i < 6; i++)
  {
    rooms[i].x = rand() % 5 + tmpX;
    rooms[i].y = rand() % 4 + tmpY;
    rooms[i].x_size = rand() % 5 + 4;
    rooms[i].y_size = rand() % 4 + 4;
    if (i == up)
    {
      dung[rooms[i].y + 2][rooms[i].x + 2] = '<';
    }
    if (i == down)
    {
      dung[rooms[i].y + 3][rooms[i].x + 3] = '>';
    }
    tmpX += 22;
    if (i == 2)
    {
      tmpX = 0;
      tmpY += 12;
    }
  }
  for (int i = 0; i < 6; i++)
  {
    fillArrayRoom(rooms[i].x, rooms[i].y, rooms[i].x + rooms[i].x_size, rooms[i].y + rooms[i].y_size);
  }
  buildCorridors(rooms);
}
void fillArrayRoom(int topX, int topY, int botX, int botY)
{
  for (int i = topY; i < botY; i++)
  {
    for (int j = topX; j < botX; j++)
    {
      if (dung[i][j] == '<' || dung[i][j] == '>')
      {
      }
      else
      {
        dung[i][j] = '.';
      }
    }
  }
}
void buildCorridors(struct room *rooms)
{
  int num_corr = rand()%5 +6;
  for (int i = 0; i < num_corr; i++)
  {
    int startX = rooms[i%6].x;
    int endX = rooms[(i + 1)%6].x;
    int startY = rooms[i%6].y;
    int endY = rooms[(i + 1)% 6].y;
    if(startX> endX){
      for(startX; startX>endX; startX--){
        if(dung[startY][startX] == ' '){
          dung[startY][startX] = '#';
        }
      }
    }
    if(startX< endX){
      for(startX; startX<endX; startX++){
        if(dung[startY][startX] == ' '){
          dung[startY][startX] = '#';
        }
      }
    }
    if(startY> endY){
      for(startY; startY>endY; startY--){
        if(dung[startY][startX] == ' '){
          dung[startY][startX] = '#';
        }
      }
    }
    if(startY< endY){
      for(startY; startY<endY; startY++){
        if(dung[startY][startX] == ' '){
          dung[startY][startX] = '#';
        }
      }
    }
  }
}