#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char side_1 = '%';
char side_2 = '#';
char side_3 = '.';
char side_4 = '-';
char side_5 = '=';
char side_6 = '+';

int distance_direction = 1;
int step_direction = 1;


int step_counter_limit_d = 160;
int step_counter_forward_d = 0;
int step_counter_backward_d = 0;

int step_counter_limit = 10;
int step_counter_forward = 0;
int step_counter_backward = 0;

float A, B, C;

float baseCubeSize = 12;
int width = 143, height = 47;
float zBuffer[160 * 47];
char buffer[160 * 47];
int backgroundASCIICode = ' ';
int distanceFromCam = 200;

float verticalOffset;
float horizontalOffset;

float K1 = 40;

float incrementSpeed = 0.8;

float x, y, z;
float ooz;
int xp, yp;
int idx;

float calculateX(int i, int j, int k) {
  return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
  return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
  return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

  ooz = 1 / z;

  xp = (int)(width / 2 - horizontalOffset + K1 * ooz * x * 2);
  yp = (int)(height / 2 - verticalOffset + K1 * ooz * y);

  idx = xp + yp * width;
  if (idx >= 0 && idx < width * height) {
    if (ooz > zBuffer[idx]) {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}

void movingForward() {
  if (step_counter_forward < step_counter_limit) {
    step_counter_forward ++;
    verticalOffset --;
  } else {
    step_counter_forward = 0;
    step_direction = 0;
  }
}

void movingBackwardDistance() {
   if (step_counter_backward_d < step_counter_limit_d) {
    step_counter_backward_d ++;
    distanceFromCam ++;
  } else {
    step_counter_backward_d = 0;
    distance_direction = 1;
  }
}

void movingForwardDistance() {
  if (step_counter_forward_d < step_counter_limit_d) {
    step_counter_forward_d ++;
    distanceFromCam --;
  } else {
    step_counter_forward_d = 0;
    distance_direction = 0;
  }
}

void movingBackward() {
   if (step_counter_backward < step_counter_limit) {
    step_counter_backward ++;
    verticalOffset ++;
  } else {
    step_counter_backward = 0;
    step_direction = 1;
  }
}

void performDynamicOffset() {
  if (step_direction == 1) {
    movingForward();
  } else if (step_direction == 0) {
    movingBackward();
  }

   if (distance_direction == 1) {
    movingForwardDistance();
  } else if (distance_direction == 0) {
    movingBackwardDistance();
  }
}

int main() {
  printf("\x1b[2J");

  horizontalOffset = 0;
  verticalOffset = 0;

  while (1) {
    performDynamicOffset();

    memset(buffer, backgroundASCIICode, width * height);
    memset(zBuffer, 0, width * height * 4);

    // MARK: - First cube

    int cube_size_1 = baseCubeSize;

    for (float cubeX = -cube_size_1; cubeX < cube_size_1; cubeX += incrementSpeed) {
      for (float cubeY = -cube_size_1; cubeY < cube_size_1;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cube_size_1, side_1);
        calculateForSurface(cube_size_1, cubeY, cubeX, side_2);
        calculateForSurface(-cube_size_1, cubeY, -cubeX, side_3);
        calculateForSurface(-cubeX, cubeY, cube_size_1, side_4);
        calculateForSurface(cubeX, -cube_size_1, -cubeY, side_5);
        calculateForSurface(cubeX, cube_size_1, cubeY, side_6);
      }
    }

    /*

    // MARK: - Second cube

    int cube_size_2 = baseCubeSize / 2;
    horizontalOffset = 1 * cube_size_2;

    for (float cubeX = -cube_size_2; cubeX < cube_size_2; cubeX += incrementSpeed) {
      for (float cubeY = -cube_size_2; cubeY < cube_size_2;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cube_size_2, side_1);
        calculateForSurface(cube_size_2, cubeY, cubeX, side_2);
        calculateForSurface(-cube_size_2, cubeY, -cubeX, side_3);
        calculateForSurface(-cubeX, cubeY, cube_size_2, side_4);
        calculateForSurface(cubeX, -cube_size_2, -cubeY, side_5);
        calculateForSurface(cubeX, cube_size_2, cubeY, side_6);
      }
    }

    // MARK: - Third cube

    int cube_size_3 = baseCubeSize / 4;
    horizontalOffset = 8 * cube_size_3;

    for (float cubeX = -cube_size_3; cubeX < cube_size_3; cubeX += incrementSpeed) {
      for (float cubeY = -cube_size_3; cubeY < cube_size_3;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cube_size_3, side_1);
        calculateForSurface(cube_size_3, cubeY, cubeX, side_2);
        calculateForSurface(-cube_size_3, cubeY, -cubeX, side_3);
        calculateForSurface(-cubeX, cubeY, cube_size_3, side_4);
        calculateForSurface(cubeX, -cube_size_3, -cubeY, side_5);
        calculateForSurface(cubeX, cube_size_3, cubeY, side_6);
      }
    }

    */

    printf("\x1b[H");

    for (int k = 0; k < width * height; k++) {
      putchar(k % width ? buffer[k] : 10);
    }

    A += 0.01;
    B += 0.05;
    C += 0.00;

    usleep(8000 * 2);
  }
  return 0;
}