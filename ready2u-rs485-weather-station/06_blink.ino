void blinkSetup() {
  pinMode(2, OUTPUT);
}
void blinkEnd() {
  pinMode(2, INPUT);
}
void blinkSet() {
  blinkSetup();
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
  blinkEnd();
}

void blinkReset() {
  blinkSetup();
  digitalWrite(2, HIGH);
  delay(5000);
  digitalWrite(2, LOW);
  blinkEnd();
}

void blinkWiFiLoss() {
  blinkSetup();
  for (int i = 1; i <= 5; i++) {
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
  }

  blinkEnd();
}

void blinkWiFiConnect() {
  blinkSetup();
  digitalWrite(2, LOW);
  blinkEnd();
}