#include <Arduino.h>

const int PIN_LED = 4;
const int PIN_CDS = 0;
const int PIN_PIR = 3;
const int PIN_BUZZER = 10;

const int DARK_THRESHOLD = 1400;
const int LIGHT_THRESHOLD = 1600;

const unsigned long PIR_WARMUP_MS = 45000;
const unsigned long BUZZER_MS = 1000;

bool isDark = false;
bool buzzerActive = false;

int previousPir = LOW;

unsigned long startTime;
unsigned long buzzerStart = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PIR, INPUT);

  ledcAttach(PIN_BUZZER, 2000, 8);
  ledcWriteTone(PIN_BUZZER, 0);

  startTime = millis();
}

void loop() {
  unsigned long now = millis();

  // CDS：暗くなったらLEDを点灯
  int cdsValue = analogRead(PIN_CDS);

  if (!isDark && cdsValue < DARK_THRESHOLD) {
    isDark = true;
  } else if (isDark && cdsValue > LIGHT_THRESHOLD) {
    isDark = false;
  }

  digitalWrite(PIN_LED, isDark ? HIGH : LOW);

  // PIR：明るさとは無関係に人の動きを検知
  int pirValue = digitalRead(PIN_PIR);
  bool warmedUp = (now - startTime >= PIR_WARMUP_MS);

  bool risingEdge = (pirValue == HIGH && previousPir == LOW);

  if (warmedUp && risingEdge && !buzzerActive) {
    ledcWriteTone(PIN_BUZZER, 2000);
    buzzerActive = true;
    buzzerStart = now;
  }

  // 1秒後にブザーを停止
  if (buzzerActive && now - buzzerStart >= BUZZER_MS) {
    ledcWriteTone(PIN_BUZZER, 0);
    buzzerActive = false;
  }

  previousPir = pirValue;

  // 動作確認用
  static unsigned long lastPrint = 0;
  if (now - lastPrint >= 500) {
    Serial.printf("CDS=%d, Dark=%d, PIR=%d\n",
                  cdsValue, isDark, pirValue);
    lastPrint = now;
  }

  delay(10);
}
