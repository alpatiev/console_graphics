#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int step_direction = 1;
int step_counter_limit = 100;
int step_counter_forward = 0;
int step_counter_backward = 0;

float A, B, C;

float baseCubeSize = 12;
int width = 158, height = 52;
float zBuffer[160 * 52];
char buffer[160 * 52];
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
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

  xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2);
  yp = (int)(height / 2 + K1 * ooz * y);

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
    distanceFromCam --;
  } else {
    step_counter_forward = 0;
    step_direction = 0;
  }
}

void movingBackward() {
   if (step_counter_backward < step_counter_limit) {
    step_counter_backward ++;
    distanceFromCam ++;
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
}

int main() {
  printf("\x1b[2J");

  while (1) {
    performDynamicOffset();

    memset(buffer, backgroundASCIICode, width * height);
    memset(zBuffer, 0, width * height * 4);

    // MARK: - First cube

    int cube_size_1 = baseCubeSize;
    horizontalOffset = -2 * cube_size_1;

    for (float cubeX = -cube_size_1; cubeX < cube_size_1; cubeX += incrementSpeed) {
      for (float cubeY = -cube_size_1; cubeY < cube_size_1;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cube_size_1, '@');
        calculateForSurface(cube_size_1, cubeY, cubeX, '$');
        calculateForSurface(-cube_size_1, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cube_size_1, '#');
        calculateForSurface(cubeX, -cube_size_1, -cubeY, ';');
        calculateForSurface(cubeX, cube_size_1, cubeY, '+');
      }
    }

    // MARK: - Second cube

    int cube_size_2 = baseCubeSize / 2;
    horizontalOffset = 1 * cube_size_2;

    for (float cubeX = -cube_size_2; cubeX < cube_size_2; cubeX += incrementSpeed) {
      for (float cubeY = -cube_size_2; cubeY < cube_size_2;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cube_size_2, '@');
        calculateForSurface(cube_size_2, cubeY, cubeX, '$');
        calculateForSurface(-cube_size_2, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cube_size_2, '#');
        calculateForSurface(cubeX, -cube_size_2, -cubeY, ';');
        calculateForSurface(cubeX, cube_size_2, cubeY, '+');
      }
    }

    // MARK: - Third cube

    int cube_size_3 = baseCubeSize / 4;
    horizontalOffset = 8 * cube_size_3;

    for (float cubeX = -cube_size_3; cubeX < cube_size_3; cubeX += incrementSpeed) {
      for (float cubeY = -cube_size_3; cubeY < cube_size_3;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cube_size_3, '@');
        calculateForSurface(cube_size_3, cubeY, cubeX, '$');
        calculateForSurface(-cube_size_3, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cube_size_3, '#');
        calculateForSurface(cubeX, -cube_size_3, -cubeY, ';');
        calculateForSurface(cubeX, cube_size_3, cubeY, '+');
      }
    }

    printf("\x1b[H");

    for (int k = 0; k < width * height; k++) {
      putchar(k % width ? buffer[k] : 10);
    }

    A += 0.05;
    B += 0.05;
    C += 0.01;
    usleep(8000 * 2);
  }
  return 0;
}
