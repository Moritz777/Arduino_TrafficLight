/*
  Programm: Ampelschaltung
  Funktion: Steuert eine Ampelanlage mit Fußgängerüberweg und Rotlichblitzer
  Autor: Moritz Heinke
  Datum: 16.03.2024
   
  Pin-Belegungen:
  - carGreen: Pin für grüne Ampel für Autos
  - carYellow: Pin für gelbe Ampel für Autos
  - carRed: Pin für rote Ampel für Autos
  - walkGreen: Pin für grüne Ampel für Fußgänger
  - walkRed: Pin für rote Ampel für Fußgänger
  - buttonCycle: Pin für den Taster zum manuellen Wechseln der Ampelzustände
  - buttonSound: Pin für den Taster zur Aktivierung/Deaktivierung des Pieptons
  - flash: Pin für Blitzlicht zur Hinderniswarnung
  - trig: Pin für Ultraschall-Sensor Trigger
  - echo: Pin für Ultraschall-Sensor Echo
  - buzzer: Pin für den Piezo-Summer
*/

// Pins LED
int carGreen = 8;
int carYellow = 9;
int carRed = 10;
int walkGreen = 4;
int walkRed = 5;
int flash = 3;

// Pins Ultrasonicsensor
int trig = 6;
int echo = 7;

// Pin Buzzer
int buzzer = 13;

// Pin Button
int buttonCycle = 12;
int buttonSound = 2;

// Konstanten
int buzzerInterval = 2000;
bool isChanged = false;

unsigned long lastCycle = 0;
unsigned long timeStartRed = 0;
unsigned long lastBuzzer = 0;
unsigned long timeToWait = 0;
unsigned long timeWait = 0;
unsigned long yellowRedTimer = 0;
unsigned long yellowTimer = 0;

void setup() {
  // Initialisiere die Pin-Modi
  pinMode(carGreen, OUTPUT);
  pinMode(carYellow, OUTPUT);
  pinMode(carRed, OUTPUT);
  pinMode(walkGreen, OUTPUT);
  pinMode(walkRed, OUTPUT);
  pinMode(buttonCycle, INPUT);
  pinMode(buttonSound, INPUT);
  pinMode(flash, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // Schalte die Ampelzustände auf Zustand: Grün
  digitalWrite(carGreen, HIGH);
  digitalWrite(walkRed, HIGH);

  // Überprüfe den Sound-Button, um den Piepton zu aktivieren/deaktivieren
  if (digitalRead(buttonSound) == HIGH) {
    delay(1000); 
    if (buzzer == 13) {
      buzzer = 11; // Pin 11 nicht belegt, Zustand: ausgeschaltet
    } 
    else {
      buzzer = 13;
    }
  }

  // Spiele alle 'soundspeed'-Millisekunden einen Piepton ab
  if (millis() - lastBuzzer >= buzzerInterval) {
    tone(buzzer, 95, 20);
    lastBuzzer = millis();
  }

  // Überprüfe den Button zum Wechseln der Ampelzustände
  if (digitalRead(buttonCycle) == HIGH) {
    // Konfiguriere die Zeit zum Warten
    if (millis() - lastCycle >= 30000) { // timer 
      timeToWait = 10000;
    }
    else{
      timeToWait = 30000 - (millis() - lastCycle);
    }

    // Schalte die Ampelzustände auf Zustand: Warten
    timeWait = millis();
    while (millis() < timeWait + timeToWait ) {
      // Spiele alle 'soundspeed'-Millisekunden einen Piepton ab
      if (millis() - lastBuzzer >= buzzerInterval) {
        tone(buzzer, 95, 20);
        lastBuzzer = millis();
      }
    }
  
    // Schalte die Ampelzustände auf Zustand: Gelb
    yellowTimer = millis();
    digitalWrite(carGreen, LOW);
    digitalWrite(carYellow, HIGH);
    while (millis() - yellowTimer <= 2000){
      if (millis() - lastBuzzer >= buzzerInterval) {
        tone(buzzer, 95, 20);
        lastBuzzer = millis();
      }
    }

    // Schalte die Ampelzustände auf Zustand: Rot
    digitalWrite(carYellow, LOW);
    digitalWrite(walkRed, LOW);
    digitalWrite(carRed, HIGH);
    digitalWrite(walkGreen, HIGH);
    timeStartRed = millis();
    buzzerInterval = 500;

    // Überwache die Fußgängerampelphase
    while (millis() - timeStartRed < 15000) {
      // Überprüfe den Übergang von Grün zu Rot und blitze bei geringem Abstand
      if (!isChanged && (millis() - timeStartRed > 12000)) {
        isChanged = true;
        digitalWrite(walkRed, HIGH);
        digitalWrite(walkGreen, LOW);
        buzzerInterval = 2000;
      }
      
      // Spiele alle 'soundspeed'-Millisekunden einen Piepton ab
      tone(buzzer, 95, 20);
      delay(buzzerInterval);

      // Ultrasonic Sensor Werte abrufen
      digitalWrite(trig, HIGH);
      delayMicroseconds(1000);
      digitalWrite(trig, LOW);
      if (pulseIn(echo, HIGH) / 58 <= 10) { // Unterschreitung von 10cm,flash
        digitalWrite(flash, HIGH);
        delay(100);
        digitalWrite(flash, LOW);
      }
    } 

    // Schalte die Ampelzustände auf Zustand: Gelb-Roten Zustand
    digitalWrite(carYellow, HIGH);
    yellowRedTimer = millis();
    while (millis() - yellowRedTimer <= 3000) {
      // Spiele alle 'soundspeed'-Millisekunden einen Piepton ab
      tone(buzzer, 95, 20);
      delay(buzzerInterval);
    }
    digitalWrite(carYellow, LOW);
    digitalWrite(carRed, LOW);

    lastCycle = millis();
    isChanged = false;
  }
}
