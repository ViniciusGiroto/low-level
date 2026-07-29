#include <TrinketHidCombo.h>

enum : uint8_t {
  SIG_POWER = 2,
  SIG_TV_POWER = 170,
  SIG_TV_SET = 154,
  SIG_TV_IN = 26,
  SIG_TV_VOLUME_UP = 234,
  SIG_TV_VOLUME_DOWN = 106,
  SIG_SETUP = 194,
  SIG_APP = 240,
  SIG_VOLUME_DOWN = 8,
  SIG_VOLUME_UP = 24,
  SIG_HOME = 136,
  SIG_BACK = 152,
  SIG_MENU = 50,
  SIG_CLICK = 0,
  SIG_NUM1 = 114,
  SIG_NUM2 = 176,
  SIG_NUM3 = 48,
  SIG_NUM4 = 82,
  SIG_NUM5 = 144,
  SIG_NUM6 = 16,
  SIG_NUM7 = 98,
  SIG_NUM8 = 160,
  SIG_NUM9 = 32,
  SIG_NUM0 = 128,
  SIG_MUTE = 130,
  SIG_BACKSPACE = 66,
  SIG_OK = 200,
  SIG_ARROW_UP = 104,
  SIG_ARROW_DOWN = 88,
  SIG_ARROW_LEFT = 138,
  SIG_ARROW_RIGHT = 10,
};

volatile uint8_t sent = 0;
volatile uint8_t data_back = 0;
volatile uint32_t time_old = 0;

void setup(void) {
  // Initialize Pin 1 for the LED and turn it off quickly
  pinMode(1, OUTPUT);
  PORTB &= ~(1 << PB1);

  // Initialize Pin 2 for the IR Receiver and enable internal pull-up
  pinMode(2, INPUT_PULLUP);

  // Use P2 on the Digispark
  attachInterrupt(0, ir_read, FALLING);

  TrinketHidCombo.begin();
}

void loop(void) {
  if (sent == 1) {
    cli();
    uint8_t data_current = data_back;
    sent = 0;
    sei();

    switch (data_current) {
    case SIG_VOLUME_UP:
    case SIG_TV_VOLUME_UP:
      TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_UP);
      break;
    case SIG_VOLUME_DOWN:
    case SIG_TV_VOLUME_DOWN:
      TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_DOWN);
      break;
    case SIG_NUM0:
      TrinketHidCombo.pressKey(0, KEYCODE_0);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM1:
      TrinketHidCombo.pressKey(0, KEYCODE_1);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM2:
      TrinketHidCombo.pressKey(0, KEYCODE_2);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM3:
      TrinketHidCombo.pressKey(0, KEYCODE_3);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM4:
      TrinketHidCombo.pressKey(0, KEYCODE_4);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM5:
      TrinketHidCombo.pressKey(0, KEYCODE_5);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM6:
      TrinketHidCombo.pressKey(0, KEYCODE_6);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM7:
      TrinketHidCombo.pressKey(0, KEYCODE_7);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM8:
      TrinketHidCombo.pressKey(0, KEYCODE_8);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_NUM9:
      TrinketHidCombo.pressKey(0, KEYCODE_9);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_ARROW_UP:
      TrinketHidCombo.pressKey(0, KEYCODE_ARROW_UP);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_ARROW_DOWN:
      TrinketHidCombo.pressKey(0, KEYCODE_ARROW_DOWN);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_ARROW_LEFT:
      TrinketHidCombo.pressKey(0, KEYCODE_ARROW_LEFT);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_ARROW_RIGHT:
      TrinketHidCombo.pressKey(0, KEYCODE_ARROW_RIGHT);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_MUTE:
      TrinketHidCombo.pressMultimediaKey(MMKEY_MUTE);
      break;
    case SIG_BACKSPACE:
      TrinketHidCombo.pressKey(0, KEYCODE_BACKSPACE);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_POWER:
    case SIG_TV_POWER:
      TrinketHidCombo.pressSystemCtrlKey(SYSCTRLKEY_POWER);
      break;
    case SIG_MENU:
      TrinketHidCombo.pressMultimediaKey(MMKEY_KB_MENU);
      break;
    case SIG_BACK:
      // TrinketHidCombo.pressMultimediaKey(MMKEY_KB_MENU);
      break;
    case SIG_HOME:
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI, 0);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_OK:
      TrinketHidCombo.pressKey(0, KEYCODE_ENTER);
      TrinketHidCombo.pressKey(0, 0);
      break;
    case SIG_TV_SET:
    case SIG_TV_IN:
    case SIG_SETUP:
    case SIG_APP:
    case SIG_CLICK:
      // TODO: implement something for this buttons
      break;
    default:
      break;
    }
  }

  // Must run at least every 10 ms to keep the USB connection alive
  TrinketHidCombo.poll();
}

void ir_read(void) {
  static uint8_t x = 0;
  static uint8_t id = 0;
  static uint8_t id_inv = 0;
  static uint8_t data = 0;
  static uint8_t data_inv = 0;
  static uint8_t repeat = 0;

  uint8_t situation = 0;
  uint8_t bit = 0;
  uint32_t time = 0;
  uint32_t time_delta = 0;

  // Fast equivalent of digitalWrite(1, HIGH)
  PORTB |= (1 << PB1);

  time = micros();

  if (time_old != 0) {
    time_delta = time - time_old;

    // If > 150 ms has passed since the last edge, reset the state machine.
    if (time_delta > 150000) {
      x = 0;
      repeat = 0;
    }

    if ((time_delta > 12000) && (time_delta < 14000)) {
      x = 0;
      situation = 1;
      id = 0;
      id_inv = 0;
      data = 0;
      data_inv = 0;
      repeat = 0;
    } else if ((time_delta > 10000) && (time_delta < 12000)) {
      situation = 2;
    } else if ((time_delta > 1500) && (time_delta < 2500)) {
      situation = 3;
      bit = 1;
    } else if ((time_delta > 1000) && (time_delta < 1500)) {
      situation = 3;
      bit = 0;
    } else {
      situation = 5;
    }

    if (situation == 3) {
      if (x < 8) {
        id |= bit;
        if (x < 7)
          id <<= 1;
        x++;
      } else if (x < 16) {
        id_inv |= bit;
        if (x < 15)
          id_inv <<= 1;
        x++;
      } else if (x < 24) {
        data |= bit;
        if (x < 23)
          data <<= 1;
        x++;
      } else if (x < 32) {
        data_inv |= bit;
        if (x < 31) {
          data_inv <<= 1;
        } else {
          // Ensure data and data_inv are logical opposites
          if ((data ^ data_inv) == 0xFF) {
            sent = 1;
            data_back = data;
            repeat = 0;
          }
        }
        x++;
      }
    } else if (situation == 2) {
      if (x == 32) {
        if (repeat == 1) {
          sent = 1;
        } else {
          repeat = 1;
        }
      }
    }
  }
  time_old = time;

  // Fast equivalent of digitalWrite(1, LOW)
  PORTB &= ~(1 << PB1);
}
