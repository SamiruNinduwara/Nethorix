// Nethorix - Network Guard - Arduino side
// Pins:  green LED = D8, red LED = D10, buzzer = D11, reset button = D2 (to GND)
//
// Signals from the PC (Python app):
//   'S' = safe, 'A' = unknown device still present,
//   'B' = a NEW unknown device appeared,
//   'N' = stopped (all off),
//   'C' = reset done (3 green blinks)
// Signal to the PC:
//   'R' = reset button held for 3 seconds
//
// Behaviour:
//   - Unknown device appears  -> red LED blinks + buzzer beeps for 6 sec, then BOTH turn off
//   - Network becomes safe    -> green LED turns on for 6 sec, then turns off
//   - Every power-up / port open: quick self-test (green, red, buzzer, 0.2 sec each)
//     so you can check the wiring at a glance.

const int greenLed = 8;
const int redLed = 10;
const int buzzer = 11;
const int resetBtn = 2;           // button between D2 and GND (INPUT_PULLUP)

const int BUZZER_FREQ = 2000;     // Hz

const unsigned long ALERT_TIME = 6000;  // red LED + buzzer run this long per alert
const unsigned long GREEN_TIME = 6000;  // green LED stays on this long when the network is safe
const unsigned long HOLD_TIME = 3000;   // hold the button 3 sec to reset

char state = 'N';                 // N = all off, S = safe, A = alert
unsigned long alertStart = 0;
unsigned long greenStart = 0;
unsigned long lastBlink = 0;
bool blinkOn = false;

unsigned long pressStart = 0;
bool pressing = false;
bool resetSent = false;

bool buzzerOn = false;            // tracks the continuous buzzer tone

void buzzerSet(bool on) {
  if (on && !buzzerOn) {
    tone(buzzer, BUZZER_FREQ);
    buzzerOn = true;
  } else if (!on && buzzerOn) {
    noTone(buzzer);
    digitalWrite(buzzer, LOW);
    buzzerOn = false;
  }
}

void allOff() {
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  buzzerSet(false);
}

void confirmBlink() {             // 3 green blinks = reset done
  allOff();
  for (int i = 0; i < 3; i++) {
    digitalWrite(greenLed, HIGH); delay(150);
    digitalWrite(greenLed, LOW);  delay(150);
  }
  state = 'N';
}

void selfTest() {                 // green -> red -> buzzer, so wiring problems are obvious
  digitalWrite(greenLed, HIGH); delay(200); digitalWrite(greenLed, LOW);
  digitalWrite(redLed, HIGH);   delay(200); digitalWrite(redLed, LOW);
  tone(buzzer, BUZZER_FREQ);    delay(200); noTone(buzzer); digitalWrite(buzzer, LOW);
}

void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(resetBtn, INPUT_PULLUP);
  allOff();
  Serial.begin(9600);
  selfTest();
}

void loop() {
  // ---- signals from the app ----
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == 'B' || c == 'A') {
      // new unknown device ('B') or first sign of one ('A') -> start the 6 sec alert
      if (c == 'B' || state != 'A') {
        alertStart = millis();
        lastBlink = 0;
        blinkOn = false;
      }
      state = 'A';

    } else if (c == 'S') {
      if (state == 'A') {                       // unknown device left: 2 short happy beeps
        buzzerSet(false);
        digitalWrite(redLed, LOW);
        tone(buzzer, 1800, 120); delay(220);
        tone(buzzer, 2400, 120); delay(150);
      }
      if (state != 'S') greenStart = millis();  // network just became safe: start green 6 sec
      state = 'S';

    } else if (c == 'N') {
      state = 'N';

    } else if (c == 'C') {
      confirmBlink();
    }
  }

  // ---- reset button (hold 3 sec) ----
  if (digitalRead(resetBtn) == LOW) {
    if (!pressing) {
      pressing = true;
      pressStart = millis();
      resetSent = false;
    }
    if (!resetSent && millis() - pressStart >= HOLD_TIME) {
      Serial.write('R');
      buzzerSet(false);
      tone(buzzer, 2000, 200);
      resetSent = true;
    }
  } else {
    pressing = false;
  }

  // ---- outputs ----
  if (state == 'A') {
    digitalWrite(greenLed, LOW);
    if (millis() - alertStart < ALERT_TIME) {
      // first 6 sec: red blinks and buzzer beeps together
      if (millis() - lastBlink >= 250) {
        lastBlink = millis();
        blinkOn = !blinkOn;
        digitalWrite(redLed, blinkOn ? HIGH : LOW);
        buzzerSet(blinkOn);
      }
    } else {
      // after 6 sec: red and buzzer off (even though the device is still there)
      digitalWrite(redLed, LOW);
      buzzerSet(false);
    }

  } else if (state == 'S') {
    // green on for 6 sec, then off
    digitalWrite(greenLed, (millis() - greenStart < GREEN_TIME) ? HIGH : LOW);
    digitalWrite(redLed, LOW);
    buzzerSet(false);

  } else {
    allOff();
  }
}
