// from https://www.youtube.com/watch?v=p09i_hoFdd0
// from https://github.com/servetgulnaroglu/cube.c

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

float A, B, C;

const int width = 160;
const int height = 44;
float zBuffer[width * height];
char buffer[width * height];
const int backgroundASCIICode = '.';
float horizontalOffset;
float verticalOffset;
const float K1 = 35;
const int distanceFromCam = 100;

// (x, y, z) = (i, j, k) * Rx(A) * Ry(B) * Rz(C)

float calculateX(float i, float j, float k) {
  return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(float i, float j, float k) {
  return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

float calculateZ(float i, float j, float k) {
  return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
  float x, y, z;
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

  float ooz; // means one over z (1/z)
  ooz = 1 / z;

  int xp, yp;
  
  xp = (int)(width / 2.0f + horizontalOffset + K1 * ooz * x * 2);
  yp = (int)(height / 2.0f + verticalOffset + K1 * ooz * y);

  int idx;
  idx = xp + yp * width;
  if (idx >= 0 && idx < width * height) {
    if (ooz >= zBuffer[idx]) {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}

void updateCubeBuffer(int cubeWidth, float incrementSpeed) {
  for (float cubeX = -cubeWidth; cubeX <= cubeWidth; cubeX += incrementSpeed) {
    for (float cubeY = -cubeWidth; cubeY <= cubeWidth; cubeY += incrementSpeed) {
      float sampleX = cubeX;
      float sampleY = cubeY;
      float sampleZ = -cubeWidth;
      calculateForSurface(sampleX, sampleY, sampleZ, '@');
      calculateForSurface(-sampleZ, sampleY, sampleX, '$');
      calculateForSurface(sampleZ, sampleY, -sampleX, '~');
      calculateForSurface(-sampleX, sampleY, -sampleZ, '#');
      calculateForSurface(sampleX, sampleZ, -sampleY, ';');
      calculateForSurface(sampleX, -sampleZ, sampleY, '+');
    }
  }
}

void renderScreenFromBuffer() {
  for (int k = 0; k < width * height; k++) {
    putchar(k % width ? buffer[k] : '\n');
  }
}

void updateDirection(float horizontalOffset, float verticalOffset, char* xDirection, char* yDirection) {
  if (horizontalOffset >= 50) {
    *xDirection = -1;
  }
  else if (horizontalOffset <= - 50) {
    *xDirection = 1;
  }
  if (verticalOffset >= 10) {
    *yDirection = -1;
  }
  else if (verticalOffset <= -10) {
    *yDirection = 1;
  }
}

void updateDegrees() {
  A += 0.05;
  B += 0.05;
  C += 0.01;
}

void resetBuffers() {
  memset(buffer, backgroundASCIICode, width * height);
  memset(zBuffer, 1, width * height * sizeof(float));
}

int main() {
  A = 0;
  B = 0;
  C = 0;
  horizontalOffset = 0;
  verticalOffset = 0;
  float cubeWidth = 20;
  float incrementSpeed = 0.5;
  char xDirection = 1;
  char yDirection = 1;
  printf("\x1b[2J");
  while (1) {
    resetBuffers();

    updateCubeBuffer(cubeWidth, incrementSpeed);
    printf("\x1b[H");

    renderScreenFromBuffer();
    
    usleep(8000 * 2);

    horizontalOffset += (0.4 * xDirection);
    verticalOffset += (0.3 * yDirection);

    updateDirection(horizontalOffset, verticalOffset, &xDirection, &yDirection);
    updateDegrees();
  }
  return 0;
}
