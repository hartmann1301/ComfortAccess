static const unsigned char PROGMEM logo_handIcon[] =
{
  B00011111, B10000000,
  B00101010, B11000000,
  B00101010, B10100000,
  B01101010, B10100000,
  B10100000, B00100000,
  B10100000, B00100000,
  B10000000, B00100000,
  B01000000, B00100000,
  B00100000, B00100000,
  B00010000, B01000000,
  B00001000, B01000000,
  B00001111, B11000000
};

static const unsigned char PROGMEM logo_warnIcon[] =
{
  B00000100, B00000000,
  B00001010, B00000000,
  B00001010, B00000000,
  B00010001, B00000000,
  B00010101, B00000000,
  B00100100, B10000000,
  B00100100, B10000000,
  B01000100, B01000000,
  B01000000, B01000000,
  B10000100, B00100000,
  B10000000, B00100000,
  B11111111, B11100000
};

enum {DETECTED_HAND, NO_HAND, UNKNOWN_HAND};
byte isHandDetected = UNKNOWN_HAND;
byte wasHandDetected = UNKNOWN_HAND;

const int handDetectThreshold = 2000;

void checkHand() {
  if (isClamp15Off) {
    // check is sensor value is above threshold
    if (valueAverage > handDetectThreshold) {
      isHandDetected = DETECTED_HAND;
    } else {
      isHandDetected = NO_HAND;
    }

  } else {
    //TODO: implement measuring while clamp 15 is on

    isHandDetected = UNKNOWN_HAND;
  }
}
