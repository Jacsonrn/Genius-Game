#include "pitches.h"
#include <Arduino.h>

// Define os pinos dos botões e LEDs
const uint8_t buttonPins[] = {0, 1, 2, 3};
const uint8_t ledPins[] = {8, 7, 6, 5};
#define SPEAKER_PIN 10

#define MAX_GAME_LENGTH 100

// Tons associados a cada botão/LED
const int gameTones[] = { NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5 };

// Sequência e índice do jogo
uint8_t gameSequence[MAX_GAME_LENGTH] = {0};
uint8_t gameIndex = 0;

void setup() {
  Serial.begin(9600);
  
  // Configura LEDs como saída e botões com pull-up interno
  for (byte i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(SPEAKER_PIN, OUTPUT);

  // Inicializa o gerador de números aleatórios
  randomSeed(analogRead(4));

  Serial.println("Simon Game Iniciado!");
}

void lightLedAndPlayTone(byte ledIndex) {
  digitalWrite(ledPins[ledIndex], HIGH);
  tone(SPEAKER_PIN, gameTones[ledIndex]);
  delay(300);
  digitalWrite(ledPins[ledIndex], LOW);
  noTone(SPEAKER_PIN);
}

void playSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(150);
  }
}

byte readButtons() {
  while (true) {
    for (byte i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        while (digitalRead(buttonPins[i]) == LOW); // aguarda soltar
        return i;
      }
    }
    delay(1);
  }
}

void gameOver() {
  Serial.print("Game Over! Sua pontuação: ");
  Serial.println(gameIndex - 1);
  gameIndex = 0;
  delay(200);

  // Efeito sonoro de falha
  tone(SPEAKER_PIN, NOTE_DS5); delay(300);
  tone(SPEAKER_PIN, NOTE_D5); delay(300);
  tone(SPEAKER_PIN, NOTE_CS5); delay(300);
  for (byte i = 0; i < 10; i++) {
    for (int pitch = -10; pitch <= 10; pitch++) {
      tone(SPEAKER_PIN, NOTE_C5 + pitch);
      delay(6);
    }
  }
  noTone(SPEAKER_PIN);
}

bool checkUserSequence() {
  for (int i = 0; i < gameIndex; i++) {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton) {
      return false;
    }
  }
  return true;
}

void playLevelUpSound() {
  tone(SPEAKER_PIN, NOTE_E4); delay(150);
  tone(SPEAKER_PIN, NOTE_G4); delay(150);
  tone(SPEAKER_PIN, NOTE_E5); delay(150);
  tone(SPEAKER_PIN, NOTE_C5); delay(150);
  tone(SPEAKER_PIN, NOTE_D5); delay(150);
  tone(SPEAKER_PIN, NOTE_G5); delay(150);
  noTone(SPEAKER_PIN);
}

void loop() {
  // Adiciona novo passo à sequência
  gameSequence[gameIndex] = random(0, 4);
  gameIndex++;
  if (gameIndex >= MAX_GAME_LENGTH) {
    gameIndex = MAX_GAME_LENGTH - 1;
  }

  Serial.print("Fase: ");
  Serial.println(gameIndex);

  // Mostra a sequência
  playSequence();

  // Verifica se o jogador acertou
  if (!checkUserSequence()) {
    gameOver();
  } else {
    playLevelUpSound();
    delay(300);
  }

  delay(500); // tempo entre rodadas
}
