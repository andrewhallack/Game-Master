#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#define LED_PIN     6
#define LED_COUNT   64

#define RIGHT_PIN   10
#define LEFT_PIN    11
#define UP_PIN      12
#define DOWN_PIN    13
#define ENTER_PIN   5
#define RESET_PIN   7

int right_state[2] = {0, 0};
int left_state[2] = {0, 0};
int up_state[2] = {0, 0};
int down_state[2] = {0, 0};
int enter_state[2] = {0, 0};
int reset_state[2] = {0, 0};

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
static uint32_t board[64];
uint32_t yellow = strip.Color(245, 239, 66);
uint32_t white = strip.Color(255, 255, 255);
uint32_t red = strip.Color(255, 0, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t black = strip.Color(0, 0, 0);

bool selectConnect4 = false;
bool selectTTT = true;
bool selectReversal = false;
bool playingConnect4 = false;
bool playingTTT = false;
bool playingReversal = false;

static int position = 7;

bool player = true;
static uint32_t pieceColor;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(12); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(RIGHT_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  pinMode(ENTER_PIN, INPUT);
  pinMode(RESET_PIN, INPUT);

  Serial.begin(9600);
}


void loop() {
  if (selectConnect4) {
    if(!playingConnect4) {
      initConnect4();
    }
    moveConnect4();
    if (checkConnect4()) {
      initConnect4();
    }
  }
  else if (selectTTT) {
    if (!playingTTT) {
      initTTT();
    }
    moveTTT();
  }
  render();
}


void render() {
  for (int i = 0; i < sizeof(board) / sizeof(board[0]); i++) {
      strip.setPixelColor(i, board[i]);
      strip.show();
  }
}



void reset() {
  reset_state[0] = digitalRead(RESET_PIN);
  if (reset_state[0] == HIGH) {
    if (reset_state[0] != reset_state[1]) {
      initConnect4();
    }
  }
  reset_state[1] = reset_state[0];
}


/*******************************************************/
//                CONNECT4 FUNCTIONS                   //
/*******************************************************/

void initConnect4() {
  player = true;
  pieceColor = red;
  position = 7;
  for (int i = 0; i < sizeof(board) / sizeof(board[0]); i++) {
      if ((i + 1) % 8 == 0) {
        board[i] = black;
      }
      else {
        board[i] = white;
      }
  }
  board[position] = pieceColor;
  playingConnect4 = true;
}


void moveConnect4() {
  rightConnect4();
  leftConnect4();
  enterConnect4();
  reset();
}


void rightConnect4() {
  right_state[0] = digitalRead(RIGHT_PIN);
  if (right_state[0] == HIGH) {
    if (right_state[0] != right_state[1]) {
      if (position < 63) {
        board[position] = black;
        position += 8;
      }
      board[position] = pieceColor;
    }
  }
  right_state[1] = right_state[0];
}


void leftConnect4() {
  left_state[0] = digitalRead(LEFT_PIN);
  if (left_state[0] == HIGH) {
    if (left_state[0] != left_state[1]) {
      if (position > 7) {
        board[position] = black;
        position -= 8;
      }
      board[position] = pieceColor;
    }
  }
  left_state[1] = left_state[0];
}


void enterConnect4() {
  bool valid = true;
  enter_state[0] = digitalRead(ENTER_PIN);
  if (enter_state[0] == HIGH) {
    if (enter_state[0] != enter_state[1]) {
      for (int i = position - 7; i <= position; i++) {
        if (board[position - 1] != white) {
          valid = false;
        }
        if (board[i] == white) {
          board[i] = pieceColor;
          break;
        }
      }

      if (valid) {
        player = !player;
      }
      if (player) {
        pieceColor = red;
      }
      else {
        pieceColor = blue;
      }
      board[position] = pieceColor;
    }
  }
  enter_state[1] = enter_state[0];
}


bool checkConnect4() {
  int directions[][6] = {{7, 64, 8, 7, 64, 1}, // i value, i max, i iterator, j subtraction, j max subtraction, j iterator
                        {0, 8, 1, 0, 8, 8},
                        {0, 33, 8, 0, 9, 9},
                        {7, 40, 8, 0, 7, 7}};

  for (int d = 0; d < 4; d++) {
    int count = 0;
    for (int i = directions[d][0]; i < directions[d][1]; i += directions[d][2]) {
      count = 0;
      for (int j = i - directions[d][3]; j < 64 - (directions[d][4] - i); j += directions[d][5]) {
         if (player) {
          if (board[j] == blue) count++;
          else count = 0;
        }
        else {
          if (board[j] == red) count++;
          else count = 0;
        }
        if (count >= 4) return true;
      }
    }
  }
  return false;
}


/*******************************************************/
//                Tic-Tac-Toe FUNCTIONS                //
/*******************************************************/


void initTTT() {
  player = true;
  pieceColor = red;
  position = 7;
  for (int i = 7; i < 64; i += 8) {
    if ((i + 1) % 3 == 0) {
      for (int j = i - 7; j <= i; j++) {
        board[j] = white;
      }
    }
    else {
      for (int j = i - 5; j < i; j += 3) {
        board[j] = white;
      }
    }
  }
  TTTpiece(position, pieceColor);
  playingTTT = true;
}


void TTTpiece(int pos, uint32_t color) {
  board[position] = color;
  board[position - 1] = color;
  board[position + 7] = color;
  board[position + 8] = color;
}


void moveTTT() {
  upTTT();
  downTTT();
  leftTTT();
  rightTTT();
}


void upTTT() {
  up_state[0] = digitalRead(UP_PIN);
  if (up_state[0] == HIGH) {
    if (up_state[0] != up_state[1]) {
      if ((position + 1) % 8 != 0) {
        TTTpiece(position, black);
        position += 3;
      }
      TTTpiece(position, pieceColor);
    }
  }
  up_state[1] = up_state[0];
}


void downTTT() {
  down_state[0] = digitalRead(DOWN_PIN);
  if (down_state[0] == HIGH) {
    if (down_state[0] != down_state[1]) {
      if ((position - 1) % 8 != 0) {
        TTTpiece(position, black);
        position -= 3;
      }
      TTTpiece(position, pieceColor);
    }
  }
  down_state[1] = down_state[0];
}


void leftTTT() {
  left_state[0] = digitalRead(LEFT_PIN);
  if (left_state[0] == HIGH) {
    if (left_state[0] != left_state[1]) {
      if (position > 7) {
        TTTpiece(position, black);
        position -= 24;
      }
      TTTpiece(position, pieceColor);
    }
  }
  left_state[1] = left_state[0];
}


void rightTTT() {
  right_state[0] = digitalRead(RIGHT_PIN);
  if (right_state[0] == HIGH) {
    if (right_state[0] != right_state[1]) {
      if (position < 49) {
        TTTpiece(position, black);
        position += 24;
      }
      TTTpiece(position, pieceColor);
    }
  }
  right_state[1] = right_state[0];
}
