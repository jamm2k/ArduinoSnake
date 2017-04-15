#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int SECTOR_WIDTH = 8;
int SECTOR_HEIGHT = 5;
int BOARD_WIDTH = 16 * SECTOR_WIDTH;
int SECTOR_WIDTH = 2 * SECTOR_HEIGHT;

int board[BOARD_WIDTH][SECTOR_WIDTH];

void setup(){
  lcd.begin(16, 2);
  Serial.begin(9600);
  init_board();
}

void init_board() {
  for(int i = 0; i < BOARD_WIDTH; i++) {
    for(int j = 0; j < BOARD_HEIGHT; j++) {
      board[i, j] = 0;
    }
  }
}

char* board_to_bytes() {
  byte new_char[BOARD_WIDTH] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000
  };
  
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < columns; j++) {
      if(matrix[i][j]) {
        byte mask = 1 << j;
        new_char[i] |= mask;
      }
    }
  }

  return new_char;
}

void loop(){
  
  lcd.createChar(0, new_char);
  lcd.home();
  lcd.write(byte(0));
}
