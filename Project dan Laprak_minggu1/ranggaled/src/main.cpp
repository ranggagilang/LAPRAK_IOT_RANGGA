#include <Arduino.h>

void setup() {
  pinMode(23, OUTPUT); // LED Merah
  pinMode(22, OUTPUT); // LED Kuning
  pinMode(21, OUTPUT); // LED Hijau
}

void loop() {
  // Lampu Merah menyala selama 20 detik
  digitalWrite(23, HIGH);
  digitalWrite(22, LOW);
  digitalWrite(21, LOW);
  delay(20000); // 20 detik

  // Lampu Kuning menyala selama 5 detik
  digitalWrite(23, LOW);
  digitalWrite(22, HIGH);
  digitalWrite(21, LOW);
  delay(5000); // 5 detik

  // Lampu Hijau menyala selama 30 detik
  digitalWrite(23, LOW);
  digitalWrite(22, LOW);
  digitalWrite(21, HIGH);
  delay(30000); // 30 detik
}
