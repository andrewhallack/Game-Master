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
#define ENTER_PIN   2

int right_state[2] = {0, 0};
int left_state[2] = {0, 0};
int up_state[2] = {0, 0};
int down_state[2] = {0, 0};
int enter_state[2] = {0, 0};

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
static uint32_t board[64];
uint32_t yellow = strip.Color(245, 239, 66);
uint32_t white = strip.Color(255, 255, 255);
uint32_t red = strip.Color(255, 0, 0);
uint32_t black = strip.Color(0, 0, 0);

bool selectConnect4 = true;
bool selectTTT = false;
bool selectReversal = false;
bool playingConnect4 = false;
bool playingTTT = false;
bool playingReversal = false;

static int position = 7;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(25); // Set BRIGHTNESS to about 1/5 (max = 255)


}


void loop() {
  if (selectConnect4) {
    if(!playingConnect4) {
      initConnect4();
    }
    move();
    // playConnect4();
  }
  render();
}


void render() {
  for (int i = 0; i < sizeof(board) / sizeof(board[0]); i++) {
      strip.setPixelColor(i, board[i]);
      strip.show();
  }
}


void initConnect4() {
  for (int i = 0; i < sizeof(board) / sizeof(board[0]); i++) {
      if ((i + 1) % 8 == 0) {
        board[i] = black;
      }
      else {
        board[i] = white;
      }
  }
  board[position] = red;
  playingConnect4 = true;
}


void move() {
  right();
  left();
}


void right() {
  right_state[0] = digitalRead(RIGHT_PIN);
  if (right_state[0] == HIGH) {
    if (right_state[0] != right_state[1]) {
      if (position < 63) {
        board[position] = black;
        position += 8;
      }
      board[position] = red;
    }
    digitalWrite(RIGHT_PIN, LOW);
  }
  right_state[1] = right_state[0];
}


void left() {
  left_state[0] = digitalRead(LEFT_PIN);
  if (left_state[0] == HIGH) {
    if (left_state[0] != left_state[1]) {
      if (position > 7) {
        board[position] = black;
        position -= 8;
      }
      board[position] = red;
    }
    digitalWrite(LEFT_PIN, LOW);
  }
  left_state[1] = left_state[0];
}
