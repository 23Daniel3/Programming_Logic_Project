#include <Servo.h>

struct Motor {
  int in1;
  int in2;
  int en;
};

Motor motores[4] = {
  {8, 9, 5},    // M1
  {10, 11, 6},  // M2
  {2, 3, 7},    // M3
  {12, 13, 4}   // M4
};

// --- Configuração do servo ---
Servo servo;
int servoPin = A0;
int servoPos = 90; // Posição inicial

void setup() {
  Serial.begin(9600);
  Serial.println("Controle de 4 motores + servo iniciado!");
  Serial.println("Comandos: M1 F 200 | M2 B 100 | S 45");

  // Configurar motores
  for (int i = 0; i < 4; i++) {
    pinMode(motores[i].in1, OUTPUT);
    pinMode(motores[i].in2, OUTPUT);
    pinMode(motores[i].en, OUTPUT);
    digitalWrite(motores[i].in1, LOW);
    digitalWrite(motores[i].in2, LOW);
    analogWrite(motores[i].en, 0);
  }

  // Configurar servo
  servo.attach(servoPin);
  servo.write(servoPos);
}

void loop() {
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando.length() == 0) return;

    if (comando.startsWith("S")) {
      int angulo;
      int args = sscanf(comando.c_str(), "S %d", &angulo);
      if (args == 1 && angulo >= 0 && angulo <= 180) {
        servoPos = angulo;
        servo.write(servoPos);
        Serial.print("Servo -> ");
        Serial.print(servoPos);
        Serial.println(" graus");
      } else {
        Serial.println("Comando servo inválido! Use: S <0-180>");
      }
      return;
    }

    char motorChar;
    char direcao;
    int velocidade = 0;
    int args = sscanf(comando.c_str(), "M%c %c %d", &motorChar, &direcao, &velocidade);
    int motorIndex = motorChar - '1'; // M1 → índice 0

    if (motorIndex < 0 || motorIndex > 3) {
      Serial.println("Motor inválido. Use M1 a M4.");
      return;
    }

    if (direcao == 'S') {
      pararMotor(motorIndex);
      Serial.print("Motor ");
      Serial.print(motorChar);
      Serial.println(" parado.");
      return;
    }

    if (velocidade < 0) velocidade = 0;
    if (velocidade > 255) velocidade = 255;

    controlarMotor(motorIndex, direcao, velocidade);
    Serial.print("Motor ");
    Serial.print(motorChar);
    Serial.print(" -> ");
    Serial.print(direcao == 'F' ? "Frente" : "Ré");
    Serial.print(" @ ");
    Serial.println(velocidade);
  }
}

void controlarMotor(int i, char direcao, int vel) {
  if (direcao == 'F') {
    digitalWrite(motores[i].in1, HIGH);
    digitalWrite(motores[i].in2, LOW);
  } else if (direcao == 'B') {
    digitalWrite(motores[i].in1, LOW);
    digitalWrite(motores[i].in2, HIGH);
  }
  analogWrite(motores[i].en, vel);
}

void pararMotor(int i) {
  digitalWrite(motores[i].in1, LOW);
  digitalWrite(motores[i].in2, LOW);
  analogWrite(motores[i].en, 0);
}
