/**
 *  Sets an alarm using a DS3231 device, goes to sleep and wakes up again
 * @author: dev-eth0
 * @url: https://www.dev-eth0.de/
 */

#include <DS3231.h> // https://github.com/NorthernWidget/DS3231
#include <Wire.h>
#include <LowPower.h> // https://github.com/rocketscream/Low-Power

DS3231 Clock;

// Some static test-date for the RTC
byte Year = 2017;
byte Month = 9;
byte Date = 17;
byte Hour = 19;
byte Minute = 29;
byte Second = 57;

// Interrupt Pin used
static const byte wakeUpPin = 2;

static const byte ledPIN= 8;

// Those are the ALARM Bits that can be used
// They need to be combined into a single value (see below)
// Found here: https://github.com/mlepard/ArduinoChicken/blob/master/roboCoop/alarmControl.ino
#define ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
#define ALRM1_MATCH_SEC        0b1110  // when seconds match
#define ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match

#define ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
#define ALRM2_MATCH_MIN        0b110   // when minutes match
#define ALRM2_MATCH_HR_MIN     0b100   // when hours and minutes match

int ledState = HIGH;
int sleepcounter = 0;

void setup() {
  // Start the serial port
  Serial.begin(115200);
  // Serial.println("Alarm Test");

  // Configure Interrupt Pin
  pinMode(wakeUpPin, INPUT_PULLUP);
  digitalWrite(wakeUpPin, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPIN, OUTPUT);
  digitalWrite(ledPIN, LOW);

  // Start the I2C interface
  Wire.begin();

  // Set time
  Clock.setClockMode(false);
  Clock.setYear(Year);
  Clock.setMonth(Month);
  Clock.setDate(Date);
  Clock.setHour(Hour);
  Clock.setMinute(Minute);
  Clock.setSecond(Second);

  // Set alarm
  Serial.println("Setting alarm");

  int ALARM_BITS = ALRM1_MATCH_SEC;

  // Trigger Alarm when Minute == 30 or 0
  // Clock.setA1Time(Day, Hour, Minute, Second, AlarmBits, DayOfWeek, 12 hour mode, PM)
  Clock.setA1Time(0, 0, 0, 1, ALARM_BITS, false, false, false); 
  Clock.turnOnAlarm(1);

  Clock.checkAlarmEnabled(1);
  Clock.checkIfAlarm(1);

  attachInterrupt(digitalPinToInterrupt(wakeUpPin), wakeUp, FALLING);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

// loop is started once the device wakes up again
void loop() {
  if(--sleepcounter<=0) {
    digitalWrite(LED_BUILTIN, LOW);
    
    Serial.println(Clock.checkAlarmEnabled(1));
    Serial.println(Clock.checkIfAlarm(1));

    Serial.println("Powering down");
    delay(100);
    
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  } else {
    blinkLED();
    Serial.print("Busy for another ");
    Serial.print(sleepcounter);
    Serial.println(" seconds");
  }

  delay(1000);
}

void blinkLED() {
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
  }

  digitalWrite(LED_BUILTIN, ledState);
}

void wakeUp() {
  // wake up again
  sleepcounter = 15;

//  delay(1000);
}
