const int POWER_SWITCH_PIN = 8;     // button to activate/deactivate alarm
const int POWER_LED_PIN = 3;        // green LED indicates alarm is ok
const int ALARM_ACTIVE_LED_PIN = 13; // red light LED indicates alarm is active, blinking alarm is starting (time to leave secured zone)

const long DEBOUNCE_DELAY = 50;     // debounce delay time

const int DOOR_SENZOR_PIN = 5;      // pin for opened door indicating

const int LEAVE_ZONE_TIME = 5000; // time to leave secured zone after alarm activating (in milliseconds)

bool powerSwitchLastState = false;  // remember last state of the power switch button
bool powerSwitchState = false;
long powerSwitchDebounceTime;

bool alarmActive = false;  // variable indicates if alarm is active=1 or inactive=0
unsigned long alarmActivatingTime = 0;
int alarmLedState = LOW;
unsigned long alarmLedHighInterval = 100;
unsigned long alarmLedLowInterval = 200;
unsigned long previousAlarmLedToggleTime = 0;

void setup() {
  Serial.begin(9600);
  // input/output PIN settings
  pinMode(POWER_SWITCH_PIN, INPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  pinMode(ALARM_ACTIVE_LED_PIN, OUTPUT);

  pinMode(DOOR_SENZOR_PIN, INPUT);

  // turn on power led
  digitalWrite(POWER_LED_PIN, HIGH);

}

void loop() {
  checkPowerSwitch();

  alarmActivating();
  

}

bool isAlarmActive() {
  return alarmActive;
}

bool isAlarmActivating() {
  return alarmActivatingTime > 0;
}

void checkPowerSwitch() {
  // turning on/off alarm
  bool reading = digitalRead(POWER_SWITCH_PIN);
  if (reading != powerSwitchLastState) {
    powerSwitchDebounceTime = millis();
  }
  if ((millis() - powerSwitchDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != powerSwitchState) {
      powerSwitchState = reading;
      if (powerSwitchState) {
        Serial.println("switching alarm");
        if (isAlarmActive() or isAlarmActivating()) turnOffAlarm();
        else turnOnAlarm();
      }
    }
  }
  powerSwitchLastState = reading;
}

void turnOffAlarm() {
  alarmActive = false;    // turn off alarm
  alarmLedState = LOW;    // turn off LED
  digitalWrite(ALARM_ACTIVE_LED_PIN, alarmLedState);
  previousAlarmLedToggleTime = millis();
  alarmActivatingTime = 0;
  Serial.println("alarm is deactivated");
}

void turnOnAlarm() {
  alarmActivatingTime = millis();
  Serial.println("turning on alarm");
}


void alarmActivating() {
  unsigned long currentMillis = millis();
  if (alarmActivatingTime > 0) {
    if (currentMillis - alarmActivatingTime < LEAVE_ZONE_TIME) {
      // blinking
      Serial.println("alarm is activating - leave zone");
      if (alarmLedState == LOW && currentMillis - previousAlarmLedToggleTime > alarmLedLowInterval) {
        alarmLedState = HIGH;
        digitalWrite(ALARM_ACTIVE_LED_PIN, alarmLedState);
        previousAlarmLedToggleTime = currentMillis;
      } else if (alarmLedState == HIGH && currentMillis - previousAlarmLedToggleTime > alarmLedHighInterval) {
        alarmLedState = LOW;
        digitalWrite(ALARM_ACTIVE_LED_PIN, alarmLedState);
        previousAlarmLedToggleTime = currentMillis;
      }
    } else {
      // LED should be lighting, alarm is ON
      alarmLedState = HIGH;
      alarmActive = true;
      digitalWrite(ALARM_ACTIVE_LED_PIN, alarmLedState);
      alarmActivatingTime = 0;
      Serial.println("alarm is active");
    }
  } 
}


