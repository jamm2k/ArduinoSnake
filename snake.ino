#include <LiquidCrystal.h>

#define SECTOR_WIDTH 5
#define SECTOR_HEIGHT 8
#define SECTORS_PER_ROW 16
#define SECTORS_PER_COLUMN 2
#define BOARD_WIDTH (SECTORS_PER_ROW * SECTOR_WIDTH)
#define BOARD_HEIGHT (SECTORS_PER_COLUMN * SECTOR_HEIGHT)

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int left_button = 6;
const int right_button = 7;

const short MIN_DRAW_WAIT = 60;

unsigned long time_left;
unsigned long time_right;

struct point_t {
  char x;
  char y;
};

struct snake_node_t {
  struct snake_node_t *next;
  struct snake_node_t *prev;
  struct point_t pos;
};

enum Direction {
  LEFT = 1,
  DOWN = LEFT << 1,
  RIGHT = LEFT << 2,
  UP = LEFT << 3,
};

struct snake_node_t* snake_head;
Direction curr_direction;
struct point_t apple_pos;

void generate_apple() {
  apple_pos.x = random(BOARD_WIDTH);
  apple_pos.y = random(BOARD_HEIGHT);
}

void init_character(byte* character) {
  for(int i = 0; i < SECTOR_HEIGHT; i++) {
  // Add a zero byte to every row
  character[i] = B00000;
  }
}

struct snake_node_t* add_snake_part() {
  struct snake_node_t* new_part = (struct snake_node_t*) malloc(sizeof(struct snake_node_t));
  struct snake_node_t* last = snake_head->next;
  snake_head->next = new_part;
  last->prev = new_part;
  new_part->next = last;
  new_part->prev = snake_head;
  return new_part;
}


void move_snake() {
  struct point_t last_pos = snake_head->pos;
  struct snake_node_t* curr_node = snake_head->prev;

  //Move head
  switch(curr_direction) {
    case LEFT:
      if(snake_head->pos.x == 0) {
        snake_head->pos.x = BOARD_WIDTH - 1;
      } else {
        snake_head->pos.x--;
      }
      break;
    case DOWN:
      if(snake_head->pos.y == (BOARD_HEIGHT - 1)) {
        snake_head->pos.y = 0;
      } else {
        snake_head->pos.y++;
      }
      break;
    case RIGHT:
      if(snake_head->pos.x == (BOARD_WIDTH - 1)) {
        snake_head->pos.x = 0;
      } else {
        snake_head->pos.x++;
      }
      break;
    case UP:
      if(snake_head->pos.y == 0) {
        snake_head->pos.y = BOARD_HEIGHT - 1;
      } else {
        snake_head->pos.y--;
      }
      break;
  }

  //Move body
  while(curr_node != snake_head) {
    struct point_t temp = curr_node->pos;
    curr_node->pos = last_pos;
    last_pos = temp;
    curr_node = curr_node->prev;
  }
}

void draw_snake() {
  byte sectors[SECTORS_PER_ROW][SECTORS_PER_COLUMN][SECTOR_HEIGHT];
  for(int i = 0; i < SECTORS_PER_COLUMN; i++) {
    for(int j = 0; j < SECTORS_PER_ROW; j++) {
      init_character(sectors[j][i]);
    }
  }

  //Apple drawing
  byte sec_x = apple_pos.x / SECTOR_WIDTH;
  byte sec_y = apple_pos.y / SECTOR_HEIGHT;
  sectors[sec_x][sec_y][apple_pos.y - (sec_y * SECTOR_HEIGHT)] |= (B10000 >> (apple_pos.x - (sec_x * SECTOR_WIDTH)));

  struct snake_node_t* curr_node = snake_head;

  do{
    struct point_t p = curr_node->pos;
    sec_x = p.x / SECTOR_WIDTH;
    sec_y = p.y / SECTOR_HEIGHT;
    sectors[sec_x][sec_y][p.y - (sec_y * SECTOR_HEIGHT)] |= (B10000 >> (p.x - (sec_x * SECTOR_WIDTH)));
    curr_node = curr_node->prev;
  }while(curr_node != snake_head);

  byte curr_sec = 0;

  for(int i = 0; i < SECTORS_PER_COLUMN; i++) {
    for(int j = 0; j < SECTORS_PER_ROW; j++) {
      bool draw_sec = false;
      for(int k = 0; k < SECTOR_HEIGHT; k++) {
        if(sectors[j][i][k]) {
          draw_sec = true;
          break;
        }
      }

      if(draw_sec) {
        lcd.createChar(curr_sec, sectors[j][i]);
        lcd.setCursor(j, i);
        lcd.write(byte(curr_sec));
        curr_sec++;
      } else {
        lcd.setCursor(j, i);
        lcd.write(" ");
      }
    }
  }
}


void setup() {
  pinMode(left_button, INPUT_PULLUP);
  pinMode(right_button, INPUT_PULLUP);
  randomSeed(analogRead(0));
  byte character[SECTOR_HEIGHT];
  lcd.begin(16, 2);
  Serial.begin(9600);
  snake_head = (struct snake_node_t*) malloc(sizeof(struct snake_node_t));
  snake_head->pos.x = 5;
  snake_head->pos.y = 5;
  snake_head->prev = snake_head;
  snake_head->next = snake_head;
  struct snake_node_t* body1 = add_snake_part();
  struct snake_node_t* body2 = add_snake_part();
  body1->pos.x = 4;
  body1->pos.y = 5;
  body2->pos.x = 3;
  body2->pos.y = 5;
  curr_direction = RIGHT;
  generate_apple();
  draw_snake();
}

bool new_down_button_state = (bool)!digitalRead(left_button);
bool new_up_button_state = (bool)!digitalRead(right_button);

short time_since_last_draw = 0;
unsigned long last_update = 0;

bool left_pressed = false;
bool right_pressed = false;

void update_input() {
  bool new_left_button_state = (bool)!digitalRead(left_button);
  bool new_right_button_state = (bool)!digitalRead(right_button);

  if(new_left_button_state){
    left_pressed = true;
    time_left = millis();
  }

  if(new_right_button_state){
    right_pressed = true;
    time_right = millis();
  }

  if((curr_direction == RIGHT) && (left_pressed) && (((millis() - time_left) > 10))) {
    curr_direction = UP;
    left_pressed = false;
  } 

  if ((curr_direction == UP) && (left_pressed)  && (((millis() - time_left) > 10))) {
    curr_direction = LEFT;
    left_pressed = false;
  }

  if((curr_direction == LEFT) && (right_pressed) && (((millis() - time_right) > 10))) {
    curr_direction = DOWN;
    right_pressed = false;
  }
  
  if((curr_direction == DOWN) && (right_pressed) && (((millis() - time_right) > 10))) {
    curr_direction = RIGHT;
    right_pressed = false;
  }
}

void loop() {

  update_input();

  unsigned long time = millis() /3;
  unsigned long elapsed = time - last_update;
  last_update = time;
  time_since_last_draw += elapsed;
  if(time_since_last_draw >= MIN_DRAW_WAIT) {
    move_snake();
    if(snake_head->pos.x == apple_pos.x && snake_head->pos.y == apple_pos.y) {
      generate_apple();
      add_snake_part();
    }
    draw_snake();
    time_since_last_draw = 0;
  }
}
