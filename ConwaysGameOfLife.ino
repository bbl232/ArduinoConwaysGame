#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define pixel_width 2
#define pixel_height 2
#define width 64
#define height 26

#define buffer_width 8

#define alive 1
#define dead 0
      
#define getCell(cells, x, y) (1 == ((cells[x/8][y] >> (x % 8)) & 1))
#define setCell(cells, x, y, state) (cells[x/8][y] = (state == 1 ? cells[x/8][y] | (1 << (x % 8)) : cells[x/8][y] & ~(1 << (x % 8))))

uint8_t cells[buffer_width][height];
uint8_t cellsNext[buffer_width][height];

unsigned int generation = 0;
unsigned int starttime;

void setup() {
  // Seed random here with the floating value from pin0
  pinMode(0, INPUT);
  randomSeed(analogRead(0));
  // Display setup stuff
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,0);

  // Title Screen
  display.println("This is Conway's\ngame of life.\n\nRules:");
  display.println("1: L < 2 = Death");
  display.println("2: L 2 or 3 = Live");
  display.println("3: L > 3 = Death");
  display.println("4: D == 3 = Birth");
  display.display();
  delay(10000);
  display.clearDisplay();
  display.display();
  
  for(int x = 0; x < width; x++){
    for (int y = 0; y < height; y++){
      setCell(cells, x, y, random(0,2));
      drawCell(x, y, getCell(cells, x, y) ? WHITE : BLACK);
    }
  }
  starttime=millis();
}

void drawCell(int x, int y, int color){
  // Changed from for loops to increase speed
  display.drawPixel((x * pixel_width), (y * pixel_height), color);
  display.drawPixel((x * pixel_width) + 1, (y * pixel_height), color);
  display.drawPixel((x * pixel_width), (y * pixel_height) + 1, color);
  display.drawPixel((x * pixel_width) + 1, (y * pixel_height) + 1, color);
}

int countNeighbours(int x, int y){
  int neighbours = 0;
  for(int i = -1; i < 2; i++){
    for(int j = -1; j < 2; j++){
      if ((x + i) < 0 || (x + i) >= width || (y + j) < 0 || (y + j) >= height || (i == 0 && j == 0)){
        continue;
      }

      if (getCell(cells, (x + i), (y + j)) == alive){
        neighbours++;
        if (neighbours == 4){
          return 4;
        }
      }
    }
  }
  return neighbours;
}

void loop() {
  generation++;
  display.setCursor(0,56);
  display.print("G:");
  display.print(generation);
  display.print(" T:");
  int secondsElapsed = (millis()-starttime)/1000;
  display.print(secondsElapsed);
  if (secondsElapsed > 0){
    display.print(" R:");
    display.print((float)generation/secondsElapsed);
  }
  // Display the current generation
  
  // While we calculate the next
  for(int x = 0; x < width; x++){
    for (int y = 0; y < height; y++){
      drawCell(x, y, getCell(cells, x, y) ? WHITE : BLACK);

      int neighbours = countNeighbours(x, y);
      int state = getCell(cells, x, y);
      
      if(state == alive && (neighbours < 2 || neighbours > 3)){
          setCell(cellsNext, x, y, dead);
      }
      else if(state == dead && (neighbours == 3)){
        setCell(cellsNext, x, y, alive);
      }
      else{
        setCell(cellsNext, x, y, state);
      }
    }
  }
  display.display(); 
  for(int x = 0; x < width; x++){
    for (int y = 0; y < height; y++){
        setCell(cells, x, y, getCell(cellsNext, x, y));
    }
  }
}
