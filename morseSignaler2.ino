#define EMPTY 0
#define DOT 1
#define DASH 2

#define msTolerance 150
#define dotLength 150
#define dashLength 650
#define dddelay 100
#define letterFinishTime 500
#define messageFinishTime 2000

unsigned long lastUpdated = 0;
unsigned long curStateTime = 0;
bool receiving = false;
bool lightWasOn = false;

int lowlevel = 300;
int highlevel = 1000;
int lightThreshold = 600;

/*unsigned int msTolerance = 5;
unsigned int dotLength = 20;
unsigned int dashLength = 40;
unsigned int dddelay = 20;
unsigned int letterFinishTime = 50;
unsigned int messageFinishTime = 500;*/

char message[128];
int messagelen = 0;
int letter[4];
int letterlen = 0;

// F0
int lightReceiver = A5;
int laser = 3;

int* charToMorseArray(char ch) {

  static int a[4] = {DOT, DASH};
  static int b[4] = {DASH, DOT, DOT, DOT};
  static int c[4] = {DASH, DOT, DASH, DOT};
  static int d[4] = {DASH, DOT, DOT};
  static int e[4] = {DOT};
  static int f[4] = {DOT, DOT, DASH, DOT};
  static int g[4] = {DASH, DASH, DOT};
  static int h[4] = {DOT, DOT, DOT, DOT};
  static int i[4] = {DOT, DOT};
  static int j[4] = {DOT, DASH, DASH, DASH};
  static int k[4] = {DASH, DOT, DASH};
  static int l[4] = {DOT, DASH, DOT, DOT};
  static int m[4] = {DASH, DASH};
  static int n[4] = {DASH, DOT};
  static int o[4] = {DASH, DASH, DASH};
  static int p[4] = {DOT, DASH, DASH, DOT};
  static int q[4] = {DASH, DASH, DOT, DASH};
  static int r[4] = {DOT, DASH, DOT};
  static int s[4] = {DOT, DOT, DOT};
  static int t[4] = {DASH};
  static int u[4] = {DOT, DOT, DASH};
  static int v[4] = {DOT, DOT, DOT, DASH};
  static int w[4] = {DOT, DASH, DASH};
  static int x[4] = {DOT, DASH, DASH};
  static int y[4] = {DASH, DOT, DASH, DASH};
  static int z[4] = {DASH, DASH, DOT, DOT};
  static int space[4] = {DOT, DOT, DASH, DASH};
  switch (ch) {
    case 'a':
    case 'A':
      return a;
    case 'b':
    case 'B':
      return b;
    case 'c':
    case 'C':
      return c;
    case 'd':
    case 'D':
      return d;
    case 'e':
    case 'E':
      return e;
    case 'f':
    case 'F':
      return f;
    case 'g':
    case 'G':
      return g;
    case 'h':
    case 'H':
      return h;
    case 'i':
    case 'I':
      return i;
    case 'j':
    case 'J':
      return j;
    case 'k':
    case 'K':
      return k;
    case 'l':
    case 'L':
      return l;
    case 'm':
    case 'M':
      return m;
    case 'n':
    case 'N':
      return n;
    case 'o':
    case 'O':
      return o;
    case 'p':
    case 'P':
      return p;
    case 'q':
    case 'Q':
      return q;
    case 'r':
    case 'R':
      return r;
    case 's':
    case 'S':
      return s;
    case 't':
    case 'T':
      return t;
    case 'u':
    case 'U':
      return u;
    case 'v':
    case 'V':
      return v;
    case 'w':
    case 'W':
      return w;
    case 'x':
    case 'X':
      return x;
    case 'y':
    case 'Y':
      return y;
    case 'z':
    case 'Z':
      return z;
    case ' ':
    case '-':
    case '_':
    default:
      return space;
  }
}

/*
   given an array of DOT and DASH, finds the corresponding ASCII letter
*/
char getLetter(int *morse, int len) {
  for (char i = 'a'; i < 'z'; i++) {
    if (morseCharsEqual(morse, charToMorseArray(i), len)) {
      return i;
    }
  }
  return ' ';
}

/*
   returns 1 if the first N elements of the integer arrays are equal, 0 otherwise
*/
bool morseCharsEqual(int *input, int *reference, int n) {
  int i = 0;
  for (i; i < n; i++) {
    if (input[i] != reference[i]) {
      return 0;
    }
  }
  for (i; i < 4; i++) {
    if (reference[i] != EMPTY) {
      return 0;
    }
  }
  return 1;
}

void sendMorseString(String toSend) {
  Serial.println("Sending...");
  bool first = true;
  for (int i = 0; toSend[i] != '\0'; i++) {
    if (!first)
      delay(letterFinishTime);
    first = false;
    Serial.write(toSend[i]);
    sendMorseStringLetter(charToMorseArray(toSend[i]));
  }
  Serial.write("\nMessage Sent!\n");
}

void sendMorseStringLetter(int* letter) {
  for (int i = 0; i < 4; i++) {
    if (letter[i] != EMPTY) {
      if (letter[i] == DOT) {
        digitalWrite(laser, HIGH);
        delay(dotLength);
      } else {
        if (letter[i] == DASH) {
          digitalWrite(laser, HIGH);
          delay(dashLength);
        } else {
          Serial.println("There was an error: unrecognized morse integer in sendMorseStringLetter");
        }
      }
      digitalWrite(laser, LOW);
      // delay between dashes and dots
      delay(dddelay);
    }
  }
}

void printMessageToSerial() {
  message[messagelen] = '\0';
  Serial.print(message);
  Serial.write('\n');
}

bool withinTolerance(int num1, int num2) {
  return abs(num1 - num2) <= msTolerance;
}

bool withinTolerance(unsigned int num1, unsigned int num2) {
  return abs(num1 - num2) <= msTolerance;
}

void calibrate() {
  Serial.println("Entered CALIBRATE mode. Type \"EXIT\" to return to normal mode or \"HELP\" for more information");
  while (1) {
    if (Serial.available()) {
      String str = Serial.readString();
      if (str == "LASER ON" || str == "ON") {
        Serial.println("Turning on laser...");
        digitalWrite(laser, HIGH);
      }
      if (str == "LASER OFF" || str == "OFF") {
        Serial.println("Turning off laser...");
        digitalWrite(laser, LOW);
      }
      if (str == "SET DARK" || str == "DARK") {
        lowlevel = analogRead(lightReceiver);
        Serial.print("Set dark level to: ");
        Serial.println(lowlevel);
        lightThreshold = (highlevel + lowlevel) / 2;
      }
      if (str == "SET LIGHT" || str == "LIGHT" || str == "SET BRIGHT" || str == "BRIGHT") {
        highlevel = analogRead(lightReceiver);
        Serial.print("Set bright level to: ");
        Serial.println(highlevel);
        lightThreshold = (highlevel + lowlevel) / 2;
      }
      if (str == "HELP") {
        Serial.println("The following commands are valid:");
        Serial.println("LASER ON | ON");
        Serial.println("  -Turns on the laser (for aiming)");
        Serial.println("LASER OFF | OFF");
        Serial.println("  -Turns off the laser");
        Serial.println("SET DARK | DARK");
        Serial.println("  -Calibrates the lower light level (use when sensor is not receiving light)");
        Serial.println("SET LIGHT | LIGHT | SET BRIGHT | BRIGHT");
        Serial.println("  -Calibrates the upper light level (use when sensor is receiving light)");
        Serial.println("HELP");
        Serial.println("  -Displays this menu");
        Serial.println("EXIT");
        Serial.println("  -Exits CALIBRATE mode and returns to normal mode");
      }
      if (str == "EXIT") {
        Serial.println("Exiting CALIBRATE mode...");
        digitalWrite(laser, LOW);
        return;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);

  // configure low level upon start
  lowlevel = analogRead(lightReceiver);
  Serial.print("Set dark level to: ");
  Serial.println(lowlevel);
  lightThreshold = (highlevel + lowlevel) / 2;
  Serial.print("Light threshold is: ");
  Serial.println(lightThreshold);
  /*Serial.print("Welcome! To use this morse transmission device, just type");
  Serial.print("your message into the terminal and press ENTER (valid");
  Serial.print("characters are a-z (lowercase) and SPACE). Messages are");
  Serial.print("automatically recieved and decoded (although this program");
  Serial.print("can't send and receive a message at the same time). Calibration");
  Serial.print("is suggested before use; to begin calibration, type CALIBRATE.");*/
  
}

void loop() {

  int lightlevel = analogRead(lightReceiver);
  int receivingLight = lightlevel >= lightThreshold;
  //int receivingLight = 0;
  /*Serial.print("Receiving light: ");
    Serial.print(receivingLight);
    Serial.write('\n');*/

  if (!receiving) {

    if (Serial.available()) {
      //Serial.println("received user text");
      String input = Serial.readString();
      Serial.println(input);
      if (input == "") {
        Serial.println("input is empty!");
      }
      if (input != "CALIBRATE" && input != "CONFIGURE") {
        sendMorseString(input);
      } else {
        calibrate();
      }
    } else {
      if (receivingLight) {
        //Serial.println("Started receiving transmission...");
        lightWasOn = true;
        receiving = true;
        curStateTime = 0;
      }
    }
  } else {

    // we're getting a signal
    if (lightWasOn && receivingLight) {
      curStateTime += (millis() - lastUpdated);
    } else {

      // the light is and was off, but we had been receiving
      if (!lightWasOn && !receivingLight) {
        curStateTime += (millis() - lastUpdated);

        // the letter is done and we received valid data
        if (curStateTime > letterFinishTime && letterlen > 0) {
          message[messagelen] = getLetter(letter, letterlen);
          messagelen++;
          letterlen = 0;
        }

        // the message being sent is done
        if (curStateTime > messageFinishTime && receiving) {
          //Serial.println("done receiving message");
          printMessageToSerial();
          messagelen = 0;
          receiving = false;
        }
      } else {

        if (lightWasOn && !receivingLight) {

          //Serial.println("within tolerance:");
          //Serial.println(withinTolerance(curStateTime, dotLength));
          if (withinTolerance(curStateTime, dotLength)) {
            if (letterlen < 4) {
              //Serial.println("DOT");
              letter[letterlen] = DOT;
              letterlen++;
            } else {
              Serial.println("Corrupted letter!");
              letterlen = 0;
            }
          } else {
            if (withinTolerance(curStateTime, dashLength)) {
              if (letterlen < 4) {
                //Serial.println("DASH");
                letter[letterlen] = DASH;
                letterlen++;
              } else {
                Serial.println("Corrupted letter!");
                letterlen = 0;
              }
            }
          }

          curStateTime = 0;
          lightWasOn = false;
        } else {

          if (!lightWasOn && receivingLight) {
            curStateTime = 0;
            lightWasOn = true;
          }

        }
      }
    }
  }
  lastUpdated = millis();
}
