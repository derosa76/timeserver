/*
void ntpd_setup() {
  // Avvio server UDP sulla porta 123
  ntpUDP.begin(123);
}

void ntpd_run() {
  byte packetBuffer[48];

  // Controllo se ci sono pacchetti NTP in arrivo
  int packetSize = ntpUDP.parsePacket();
  
  if (packetSize == 48) {
    // Lettura pacchetto ricevuto
    ntpUDP.read(packetBuffer, 48);
    
    // Preparazione risposta NTP
    packetBuffer[0] = 0b00100100;    // LI, Version, Mode
    packetBuffer[1] = 1;             // Stratum
    packetBuffer[2] = 6;             // Polling Interval
    packetBuffer[3] = 0xEC;          // Precision
    
    // Ottieni il timestamp dall'oggetto time_object_gps
    struct timeval tv;
    tv.tv_sec = time_object_gps.getEpoch();
    tv.tv_usec = 0;
    
    // Converti in formato NTP (aggiunge i secondi dal 1900 al 1970)
    uint32_t ntpTime = tv.tv_sec + 2208988800UL;
    
    // Inserimento timestamp nella risposta
    packetBuffer[40] = (ntpTime >> 24) & 0xFF;
    packetBuffer[41] = (ntpTime >> 16) & 0xFF;
    packetBuffer[42] = (ntpTime >> 8) & 0xFF;
    packetBuffer[43] = ntpTime & 0xFF;
    
    // Invio risposta
    ntpUDP.beginPacket(ntpUDP.remoteIP(), ntpUDP.remotePort());
    ntpUDP.write(packetBuffer, 48);
    ntpUDP.endPacket();
    
    Serial.println("Richiesta NTP processata");
  }
}
*/

