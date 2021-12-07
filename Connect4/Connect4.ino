#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//Defining global pin Constants
#define LED_PIN     6
#define LED_COUNT   64

#define RIGHT_PIN   10
#define LEFT_PIN    11
#define DOWN_PIN    12
#define UP_PIN      13
#define ENTER_PIN   9
#define RESET_PIN   8

int right_state[2] = {0, 0};
int left_state[2] = {0, 0};
int up_state[2] = {0, 0};
int down_state[2] = {0, 0};
int enter_state[2] = {0, 0};
int reset_state[2] = {0, 0};

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
static bool validReversal[64];
static uint32_t board[64];
uint32_t purple = strip.Color(255, 0, 255);
uint32_t white = strip.Color(255, 255, 255);
uint32_t red = strip.Color(255, 0, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t black = strip.Color(0, 0, 0);

bool menu = true;
bool selectConnect4 = false;
bool selectTTT = false;
bool selectReversal = false;
bool playingConnect4 = false;
bool playingTTT = false;
bool playingReversal = false;

static int position = 7;

bool player = true;
static uint32_t pieceColor;
int menuOptions = 1;


void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(RIGHT_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  pinMode(ENTER_PIN, INPUT);
  pinMode(RESET_PIN, INPUT);

  Serial.begin(9600);
}


void loop() {
  if (menu) {
    initMenu();
    moveMenu();
  }
  else if (selectConnect4) {
    if(!playingConnect4) {
      initConnect4();
    }
    moveConnect4();
  }
  else if (selectTTT) {
    if (!playingTTT) {
      initTTT();
    }
    moveTTT();
  }
  else if (selectReversal) {
    if (!playingReversal) {
      initReversal();
    }
    moveReversal();
  }
  render();
  reset();
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
      resetVariables();
    }
  }
  reset_state[1] = reset_state[0];
}


void resetVariables() {
  menu = true;
  selectConnect4 = false;
  selectTTT = false;
  selectReversal = false;
  playingConnect4 = false;
  playingTTT = false;
  playingReversal = false;
  menuOptions = 1;
  initMenu();
}


/*******************************************************/
//                MENU FUNCTIONS                       //
/*******************************************************/


void initMenu() {
  if (menuOptions % 3 == 1) {
    connect4Menu();
  }
  else if (menuOptions % 3 == 2) {
    tttMenu();
  }
  else if (menuOptions % 3 == 0) {
    reversalMenu();
  }
}


void resetMenu() {
  for (int i = 0; i < 64; i++) {
    board[i] = black;
  }
}


void connect4Menu() {
  resetMenu();
  int fourSquares[11] = {20, 21, 22, 28, 36, 41, 42, 43, 44, 45, 46};
  for (int i = 0; i < 11; i++) {
    for (int j = 0; j < 64; j++) {
      if (j == fourSquares[i]) {
        board[j] = white;
        continue;
      }
      if (board[j] != white && j <= 31) board[j] = red;
      else if (board[j] != white && j > 31) board[j] = blue;
    }
  }
}


void tttMenu() {
  resetMenu();
  for (int i = 7; i < 57; i += 7) {
    board[i] = white;
  }
  for (int i = 4; i < 32; i += 9) {
    board[i] = white;
  }
  for (int i = 32; i < 60; i += 9) {
    board[i] = white;
  }

  int leftO[12] = {0, 1, 2, 3, 8, 11, 16, 19, 24, 25, 26 ,27};
  int rightO[12] = {36, 37, 38, 39, 44, 47, 52, 55, 60, 61, 62, 63};
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 64; j++) {
      if (j == leftO[i] || j == rightO[i]) {
        board[j] = white;
        continue;
      }
      if (board[j] != white && j <= 31) board[j] = red;
      else if (board[j] != white && j > 31) board[j] = blue;
    }
  }
}


void reversalMenu() {
  resetMenu();
  for (int i = 0; i < 64; i++) {
    board[i] = white;
  }
  
  board[27] = red;
  board[36] = red;
  board[28] = blue;
  board[35] = blue;
}


void moveMenu() {
  rightMenu();
  leftMenu();
  enterMenu();
}


void rightMenu() {
  right_state[0] = digitalRead(RIGHT_PIN);
  if (right_state[0] == HIGH) {
    if (right_state[0] != right_state[1]) {
      menuOptions += 1;
      if (menuOptions > 3) {
        menuOptions = 1;
      }
      Serial.print(menuOptions);
    }
  }
  right_state[1] = right_state[0];
}


void leftMenu() {
  left_state[0] = digitalRead(LEFT_PIN);
  if (left_state[0] == HIGH) {
    if (left_state[0] != left_state[1]) {
      menuOptions -= 1;
      if (menuOptions < 1) {
        menuOptions = 3;
      }
      Serial.print(menuOptions);
    }
  }
  left_state[1] = left_state[0];
}


void enterMenu() {
  enter_state[0] = digitalRead(ENTER_PIN);
  if (enter_state[0] == HIGH) {
    if (enter_state[0] != enter_state[1]) {
      menu = false;
      for (int i = 0; i < 64; i++) {
        board[i] = black;
      }
      if (menuOptions % 3 == 1) {
        selectConnect4 = true;
      }
      else if (menuOptions % 3 == 2) {
        selectTTT = true;
      }
      else if (menuOptions % 3 == 0) {
        selectReversal = true;
      }
    }
  }
  enter_state[1] = enter_state[0];
}


void endScreen(bool tie) {
  if (tie) {
    for (int i; i < 64; i++) {
      board[i] = purple;
    }
  }
  else if (player) {
    for (int i; i < 64; i++) {
      board[i] = blue;
    }
  }
  else {
    for (int i; i < 64; i++) {
      board[i] = red;
    }
  }
  render();
  delay(2000);
  resetVariables();
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
      checkConnect4();
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
        if (count >= 4) {
          endScreen(false);
          return true;
        }
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
  for (int i = 0; i < 64; i++) {
    if (board[i] != white) {
      board[i] = black;
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
  enterTTT();
}


void upTTT() {
  up_state[0] = digitalRead(UP_PIN);
  if (up_state[0] == HIGH) {
    if (up_state[0] != up_state[1]) {
      for (int i = position;; i += 3) {
        if (board[i] == black) {
          TTTpiece(position, black);
          position = i;
          break;
        }
        if ((i + 1) % 8 == 0) break;
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
      for (int i = position;; i -= 3) {
        if (board[i] == black) {
          TTTpiece(position, black);
          position = i;
          break;
        } 
        if ((i - 1) % 8 == 0) break;
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
      for (int i = position; i > 0; i -= 24) {
        if (board[i] == black) {
          TTTpiece(position, black);
          position = i;
          break;
        }
      }
      TTTpiece(position, pieceColor);
//      if (position > 7) {
//        TTTpiece(position, black);
//        position -= 24;
//      }
//      TTTpiece(position, pieceColor);
    }
  }
  left_state[1] = left_state[0];
}


void rightTTT() {
  right_state[0] = digitalRead(RIGHT_PIN);
  if (right_state[0] == HIGH) {
    if (right_state[0] != right_state[1]) {
      for (int i = position; i < 56; i += 24) {
        if (board[i] == black) {
          TTTpiece(position, black);
          position = i;
          break;
        }
      }
      TTTpiece(position, pieceColor);
//      if (position < 49) {
//        TTTpiece(position, black);
//        position += 24;
//      }
//      TTTpiece(position, pieceColor);
    }
  }
  right_state[1] = right_state[0];
}


void enterTTT() {
  enter_state[0] = digitalRead(ENTER_PIN);
  if (enter_state[0] == HIGH) {
    if (enter_state[0] != enter_state[1]) {
      // TTTpiece(position, pieceColor);

      player = !player;
      if (player) {
        pieceColor = red;
      }
      else {
        pieceColor = blue;
      }

      for (int i = 7; i < 56; i += 24) {
        for (int j = i - 6; j <= i; j += 3) {
          if (board[j] == black) {
            position = j;
            break;
          }
        }
      }
      checkTTT();
      TTTpiece(position, pieceColor);
    }
  }
  enter_state[1] = enter_state[0];
}


bool checkTTT() {
  uint32_t prevColor = NULL;
  int count = 1;

  int directions[][6] = {{7, 56, 24, 6, 0, 3},
                         {1, 8, 3, 0, 48, 24},
                         {7, 8, 1, 0, 50, 21},
                         {1, 2, 1, 0, 54, 27}};

  for (int k = 0; k < 4; k++) {
    for (int i = directions[k][0]; i < directions[k][1]; i += directions[k][2]) {
      prevColor = NULL;
      count = 1;
      for (int j = i - directions[k][3]; j <= i + directions[k][4]; j += directions[k][5]) {
        if (board[j] == black) {
          break;
        }
        if (board[j] == prevColor) {
          count++;
        }
        prevColor = board[j];
        if (count == 3) {
          endScreen(false);
          return true;
        }
      }
    }
  }
  return false;
}


/******************************************************/
//                REVERSAL FUNCTIONS                  //
/******************************************************/


void initReversal() {
  player = true;
  pieceColor = red;
  position = 21;

  for (int i = 0; i < sizeof(board) / sizeof(board[0]); i++) {
    board[i] = white;
    validReversal[i] = false;
  }

  board[27] = red;
  board[36] = red;
  board[28] = blue;
  board[35] = blue;

  for (int i = 18; i < 50; i += 8) {
    for (int j = i; j < i + 4; j++) {
      if (board[j] == white) {
        validReversal[j] = true;
      }
    }
  }
  
  board[position] = pieceColor;
  playingReversal = true;
}


void moveReversal() {
  leftReversal();
  rightReversal();
  upReversal();
  downReversal();
  enterReversal();
}


void leftReversal() {
  left_state[0] = digitalRead(LEFT_PIN);
  if (left_state[0] == HIGH) {
    if (left_state[0] != left_state[1]) {
      if (position > 7) {
        for (int i = position - 8; i > 0; i -= 8) {
          if (validReversal[i]) {
            board[position] = white;
            position = i;
            board[position] = pieceColor;
            break;
          }
        }
      }
    }
  }
  left_state[1] = left_state[0];
}


void rightReversal() {
  right_state[0] = digitalRead(RIGHT_PIN);
  if (right_state[0] == HIGH) {
    if (right_state[0] != right_state[1]) {
      if (position < 56) {
        for (int i = position + 8; i < 64; i += 8) {
          if (validReversal[i]) {
            board[position] = white;
            position = i;
            board[position] = pieceColor;
            break;
          }
        }
      }
    }
  }
  right_state[1] = right_state[0];
}


void upReversal() {
  up_state[0] = digitalRead(UP_PIN);
  if (up_state[0] == HIGH) {
    if (up_state[0] != up_state[1]) {
      if ((position + 1) % 8 != 0) {
        for (int i = position + 1;; i++) {
          if (i % 8 == 0) break;
          if (validReversal[i]) {
            board[position] = white;
            position = i;
            board[position] = pieceColor;
            break;
          }
        }
      }
    }
  }
  up_state[1] = up_state[0];
}


void downReversal() {
  down_state[0] = digitalRead(DOWN_PIN);
  if (down_state[0] == HIGH) {
    if (down_state[0] != down_state[1]) {
      if (position % 8 != 0) {
        for (int i = position - 1;; i--) {
          if ((i + 1) % 8 == 0) break;
          if (validReversal[i]) {
            board[position] = white;
            position = i;
            board[position] = pieceColor;
            break;
          }
        }
      }
    }
  }
  down_state[1] = down_state[0];
}


void enterReversal() {
  enter_state[0] = digitalRead(ENTER_PIN);
  if (enter_state[0] == HIGH) {
    if (enter_state[0] != enter_state[1]) {
      // change color
      // set valid squares
      // place piece on closest valid position
      
      player = !player;
      
      if (player) {
        pieceColor = red;
      }
      else {
        pieceColor = blue;
      }
      
      if (position == 0) {
        if (board[1] == white) validReversal[1] = true;
        if (board[8] == white) validReversal[8] = true;
        if (board[9] == white) validReversal[9] = true;
      }
      else if (position == 7) {
        if (board[6] == white) validReversal[6] = true;
        if (board[14] == white) validReversal[14] = true;
        if (board[15] == white) validReversal[15] = true;
      }
      else if (position == 56) {
        if (board[48] == white) validReversal[48] = true;
        if (board[49] == white) validReversal[49] = true;
        if (board[57] == white) validReversal[57] = true;
      }
      else if (position == 63) {
        if (board[54] == white) validReversal[54] = true;
        if (board[55] == white) validReversal[55] = true;
        if (board[62] == white) validReversal[62] = true;
      }
      else if ((position + 1) % 8 == 0) {
        for (int i = position - 8; i <= position + 8; i += 8) {
          for (int j = i; j >= i - 1; j--) {
            if (board[j] == white) validReversal[j] = true;
          }
        }
      }
      else if (position % 8 == 0) {
        for (int i = position - 8; i <= position + 8; i += 8) {
          for (int j = i; j <= i + 1; j++) {
            if (board[j] == white) validReversal[j] = true;
          }
        }
      }
      else if (position < 7) {
        for (int i = position - 1; i <= position + 7; i += 8) {
          for (int j = i; j <= i + 2; j++) {
            if (board[j] == white) validReversal[j] = true;
          }
        }
      }
      else if (position > 56) {
        for (int i = position - 9; i <= position - 1; i += 8) {
          for (int j = i; j <= i + 2; j++) {
            if (board[j] == white) validReversal[j] = true;
          }
        }
      }
      else {
        for (int i = position - 9; i <= position + 7; i += 8) {
          for (int j = i; j <= i + 2; j++) {
            if (board[j] == white) validReversal[j] = true;
          }
        } 
      }

      flip();
      checkReversal();

      int prevPosition = position;
      for (int i = 0; i < 64; i++) {
        if (board[i] == white && validReversal[i]) {
          position = i;
          break;
        }
      }
      validReversal[prevPosition] = false;
      board[position] = pieceColor;
    }
  }
  enter_state[1] = enter_state[0];
}


void flip() {
  uint32_t curColor = board[position];

  int posDirections[][4] = {{1, position + (8 - position % 8)}, {7, 64}, {8, 64}, {9, 64}};
  int negDirections[][4] = {{1, position % 8}, {7, position}, {8, position}, {9, position}};

  for (int k = 0; k < 4; k++) {
    for (int i = position - negDirections[k][0]; i >= position - negDirections[k][1]; i -= negDirections[k][0]) {
      if (board[i] == curColor) break;
      if (board[i] == white) {
        for (int j = i + negDirections[k][0]; j < position; j += negDirections[k][0]) {
          if (curColor == red) board[j] = blue;
          else if (curColor == blue) board[j] = red; 
        }
        break;
      }
      board[i] = curColor;
    }
  }

  for (int k = 0; k < 4; k++) {
    for (int i = position + posDirections[k][0]; i <= posDirections[k][1]; i += posDirections[k][0]) {
      if (board[i] == curColor) break;
      if (board[i] == white) {
        for (int j = i - posDirections[k][0]; j > position; j -= posDirections[k][0]) {
          if (curColor == red) board[j] = blue;
          else if (curColor == blue) board[j] = red; 
        }
        break;
      }
      board[i] = curColor;
    }
  }
}


void checkReversal() {
  int countValid = 0;
  int redCount = 0;
  int blueCount = 0;
  uint32_t endColor;
  
  for (int i = 0; i < 64; i++) {
    if (validReversal[i] == true) {
      countValid++;
    }
    if (board[i] == blue) blueCount++;
    if (board[i] == red) redCount++;
  }
  if (countValid == 0) {
    if (blueCount > redCount) {
      endColor = blue;
    }
    else if (redCount > blueCount) {
      endColor = red;
    }
    else {
      endColor = purple;
    }
    for (int i = 0; i < 64; i++) {
      board[i] = endColor;
    }
  }
}
