#include <LiquidCrystal.h>

#define SECTOR_WIDTH 5
#define SECTOR_HEIGHT 8
#define SECTORS_PER_ROW 16
#define SECTORS_PER_COLUMN 2
#define BOARD_WIDTH (SECTORS_PER_ROW * SECTOR_WIDTH)
#define BOARD_HEIGHT (SECTORS_PER_COLUMN * SECTOR_HEIGHT)

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int board[BOARD_HEIGHT][BOARD_WIDTH];

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  init_board();
}

void init_board() {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      board[i][j] = 0;
    }
  }
}

void init_character(byte* character) {
  for(int i = 0; i < SECTOR_HEIGHT; i++) {
    // Add a zero byte to every row
    character[i] = B00000;
  }
}

void sector_to_bytes(int start_x, int start_y, byte* character) {
  init_character(character);

  // Converts the sector to a character with bytes
  for (int i = start_y; i < start_y + SECTOR_HEIGHT; i++) {
    for (int j = start_x; j < start_x + SECTOR_WIDTH; j++) {
      // If that cell is a 1, add it to the byte of the character
      // by using a mask
      if (board[i][j]) {
        byte mask = 1 << j;
        character[i] |= mask;
      }
    }
  }
}

void draw() {
  for(int i = 0; i < BOARD_HEIGHT; i += SECTOR_HEIGHT) {
    for(int j = 0; j < BOARD_WIDTH; j += SECTOR_WIDTH) {
      // The character represents rows with bytes
      byte character[SECTOR_HEIGHT];
      
      sector_to_bytes(j, i, character);
      lcd.createChar(0, character);

      // This function needs to be called, so that the new character can be used
      lcd.home();

      // lcd.home() sets the cursor to the top left, so we need to move it to
      // where we want to draw
      lcd.setCursor(j / SECTOR_WIDTH, i / SECTOR_HEIGHT);

      // Write the new character by saying to which place it
      // was saved (the first argument of the lcd.createChar() function call
      lcd.write(byte(0));
    }
  }
}

void loop() {
  draw();
}
