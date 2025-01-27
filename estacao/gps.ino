void gpsAvailable() {
  Serial.println(F("Iniciando GPS"));
  
  bool gpsExit = false;
  
  while (!gpsExit) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        gpsExit = gpsDisplayInfo();
      }
    }
  
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      // Provavelmente seu módulo de GPS não está conectado corretamente ou o módulo está defeituoso.
      displayHeader();
      display.setTextSize(1);
      display.println("Falha no GPS");
      display.display();
      
      Serial.println(F("Sinal GPS não detectado"));
      digitalWrite(redLed, HIGH);
      delay(5000);
      digitalWrite(redLed, LOW);
      delay(5000);
    }
  }
}

bool gpsDisplayInfo() {
    Serial.println("Iniciando GPS");
    delay(2000);
    
    if (gps.location.isValid()) {      
      Serial.println("GPS Ok");
      
      return true;
    }

    displayHeader();
    display.setTextSize(1);
    display.println("GPS Sem Sinal");
    display.display();

    // Falha na obtenção do sinal, as vezes o módulo demora para pegar sinal, em último caso, tente o trocar de lugar.
    Serial.println(F("Falha na Localizacao do GPS"));
    digitalWrite(redLed, HIGH);
    delay(3000);
    digitalWrite(redLed, LOW);
    delay(3000);
  
  return false;
}
