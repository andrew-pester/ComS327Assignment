#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
char dung[21][80];
struct room{
  int x;//x coord in array
  int y;//y coord in array
  int x_size;//random x size
  int y_size;//random y size
};
int main(int argc, char *argv[]){
  srand(time(NULL));
  struct room rooms[6];
  for(int i =0; i< 21;i++){
    for(int j = 0; j<80;j++){
      dung[i][j] = ' ';
    }
  }
  makeRooms(rooms);

  for(int i =0; i< 21;i++){
    for(int j = 0; j<80;j++){
      printf("%c", dung[i][j]);
    }
    printf("\n");
  }

}
int makeRooms(struct room *rooms){
  //makes rooms in 6 different localized areas so the generation is random but rooms will never hit
  //this works for generating random rooms
  int tmpX = 0, tmpY = 0;
  int up = rand()%6;//random room from 0 - 5
  int down = rand()%6; //random room from 0 - 5
  for(int i =0; i<6;i++){
    rooms[i].x = rand()%5+tmpX;
    rooms[i].y = rand()%3+tmpY;
    rooms[i].x_size = rand()%5+4;
    rooms[i].y_size = rand()%4+4;
    if(i == up){
      dung[rooms[i].y+2][rooms[i].x+2] = '<';
    }
    if(i == down){
      dung[rooms[i].y+2][rooms[i].x+2] = '>';
    }
    tmpX += 22;
    if(i == 2){
      tmpX = 0;
      tmpY += 12;
    }
  }
  for(int i = 0; i < 6; i++)
    {
      fillArrayRoom(rooms[i].x, rooms[i].y, rooms[i].x+rooms[i].x_size, rooms[i].y+rooms[i].y_size);
  }
}
void fillArrayRoom(int topX, int topY,int botX,int botY){
  for(int i = topY; i<botY;i++){
    for(int j = topX; j<botX;j++){
      if(dung[i][j]== '<'|| dung[i][j]=='>'){
      }else{
        dung[i][j] = '.';
      }
            
    }
  }
}

/*
void fillArrayCorridor(int rm1x, int rm1y, int rm2x, int rm2y)
{
  int tempx =0;
  int tempy =0;
  bool right;
  bool up;

  if(rm1y > rm2y){
    tempy = rm1y - rm2y;
    up = true;
  } if(rm1y < rm2y){
    tempy = rm2y - rm1y;
    up = false;
  } if(rm1y == rm2y){
    tempy = 0;
    up = true;
  }

  if(rm1x > rm2x){
    tempx = rm1x - rm2x;
    right = true;
  } if(rm1y < rm2y){
    tempx = rm2x - rm1x;
    right = false;
  } if(rm1y == rm2y){
    tempx = 0;
    right = true;
  }

  if (right){
    if (up){
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x + tempx][rm1y + tempy] != ' '){
	  } else {
	    dung[rm1x + tempx][rm1y + tempy] = '#';
	  }
	}
  }
    } else {
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x + tempx][rm1y - tempy] != ' '){
	  } else {
	    dung[rm1x + tempx][rm1y - tempy] = '#';
	  }
	}
      }
    }
  } else {
    if(up){
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x - tempx][rm1y + tempy] != ' '){
	  } else {
	    dung[rm1x - tempx][rm1y + tempy] = '#';
	  }
	}
      }
      
    } else {
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x - tempx][rm1y - tempy] != ' '){
	  } else {
	    dung[rm1x - tempx][rm1y - tempy] = '#';
	  }
	}
      }
    }
  }


  for(tempx; tempx > 0; tempx--){
    for (tempy; tempy > 0; tempy--){
      if(dung[rm1x - tempx][rm1y - tempy] != ' '){
      } else {
	dung[rm1x - tempx][rm1y - tempy] = '#';
      }
    }
  }
  
}
*/