//NTP CLIENT ENHANCED with microsecond precision
//Uses native ESP32 functions: configTime() + gettimeofday()
//No external libraries needed - built into ESP32 SDK!
//FULLY COMPATIBLE with original ntp_client.ino

//*#include <sys/time.h>

// ============================================================================
// COMPATIBILITÀ CON VECCHIO CODICE
// ============================================================================

// Mantiene WiFiUDP per compatibilità con ntp_server_enhanced.ino
//*WiFiUDP ntpUDP;

// Classe wrapper per compatibilità con webserver che usa ntp_client.isTimeSet()

// class NTPClientWrapper {
//   public:
//     //bool isTimeSet() {
//     //  return ntp_time_is_set;
//     //}
    
//     unsigned long getEpochTime() {
//       return time_object_ntp.getEpoch();
//     }
// };


// Oggetto ntp_client per compatibilità
//NTPClientWrapper ntp_client;

// ============================================================================
// VARIABILI
// ============================================================================

unsigned long ntp_show_last_time = 0;
bool ntp_first_sync_done = false;
unsigned long ntp_last_successful_sync = 0;

// ============================================================================
// FUNZIONI
// ============================================================================

void ntp_cl_setup()
{
  // Configura NTP con server pool.ntp.org
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3)
  // GMT offset = 0 (usiamo UTC puro, poi aggiungiamo offset Italia manualmente)
  // Daylight offset = 0 (gestiamo ora legale manualmente)
  
  //*configTime(0, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");
  
  //*otalog("NTP: Client nativo ESP32 avviato con precisione al microsecondo");
  //*otalog("NTP: Server: pool.ntp.org, time.nist.gov, time.google.com");
}

void ntp_cl_run(unsigned long dt)
{
  // Il client NTP nativo ESP32 lavora completamente in background!
  // Non serve chiamare update() - tutto automatico
  
  if(millis() >= ntp_show_last_time + dt) {
    ntp_show_last_time = millis();
    
    // Ottieni tempo corrente con precisione al microsecondo
    //*struct timeval tv;
    //*gettimeofday(&tv, NULL);
    
    // Controlla se il tempo è stato sincronizzato
    // (se l'anno è > 2020, assumiamo che sia sincronizzato)
    //*time_t now = tv.tv_sec;
    //*struct tm timeinfo;
    //*gmtime_r(&now, &timeinfo);

    //ottengo epochtime unix (da 1-1-1970)
    double x=ntp_sync_precise("pool.ntp.org");

    otalog("unix epoch ntp="+String(x,3));

    if (x>(2020-1970)*365*24*3600) { // Tempo sincronizzato!
    //*if (timeinfo.tm_year + 1900 > 2020) { // Tempo sincronizzato!
      
      //*unsigned long epoch_seconds = tv.tv_sec;
      //*unsigned long microseconds = tv.tv_usec;

      unsigned long s = (unsigned long)x;
      unsigned int us = (unsigned int)((x - s) * 1000000);
      
      // Setta ESP32Time con microsecondi per massima precisione
      time_object_ntp.setTime(s, us);
      
      ntp_time_is_set = true;
      ntp_server_mark_ntp_sync();
      
      // Marca che abbiamo fatto almeno un sync
      if (!ntp_first_sync_done) {
        ntp_first_sync_done = true;
        ntp_last_successful_sync = millis();
        otalog("NTP: Prima sincronizzazione completata!");
      }

      ntp_offset_italia = time_offset_italia(time_object_ntp);
      
      // Stampa informazioni con microsecondi
      // unsigned long millis_part = microseconds / 1000;
      // unsigned long micros_part = microseconds % 1000;
      
      // Serial.print("NTP:" + String(time_object_ntp.getEpoch()) + 
      //              "(+" + String(ntp_offset_italia) + ") - " + 
      //              time_object_ntp.getTime("%A, %B %d %Y %H:%M:%S"));
      // Serial.print(".");
      
      // // Padding millisecondi (sempre 3 cifre)
      // if (millis_part < 100) Serial.print("0");
      // if (millis_part < 10) Serial.print("0");
      // Serial.print(String(millis_part));
      
      // // Mostra anche microsecondi (sempre 3 cifre)
      // if (micros_part < 100) Serial.print("0");
      // if (micros_part < 10) Serial.print("0");
      // Serial.println(String(micros_part) + " (±1µs)");
      
      // Informazione su ultimo sync (utile per debug) - mostra solo la prima volta
      static bool sync_info_shown = false;
      if (ntp_last_successful_sync > 0 && !sync_info_shown) {
        otalog("NTP: Sincronizzazione NTP attiva e funzionante");
        sync_info_shown = true;
      }
      
    } else {
      otalog("NTP:N/A (waiting for sync...)");
      ntp_time_is_set = false;
    }
  }
}


/*
 * Sincronizza l'orologio con un server NTP con precisione al millisecondo
 * 
 * Parametri:
 *   ntp_server - indirizzo del server NTP (es. "pool.ntp.org")
 *   
 * Ritorna:
 *   epoch Unix (GMT) come double (secondi.frazione)
 *   0.0 se errore
 *   
 * Note:
 *   - T3 (Transmit Timestamp) è il momento in cui il server ha inviato la risposta
 *   - Compensiamo con RTT/2 per stimare il tempo di trasmissione
 *   - Non calcoliamo l'offset del clock locale (richiederebbe timestamp sincronizzati)
 */
double ntp_sync_precise(const char* ntp_server) {
  WiFiUDP udp;
  const int NTP_PACKET_SIZE = 48;
  byte packetBuffer[NTP_PACKET_SIZE];
  
  // Inizializza UDP
  udp.begin(2390);
  
  // Pulisci il buffer
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  // Prepara pacchetto NTP request (RFC 5905)
  packetBuffer[0] = 0b11100011;   // LI=3, Version=4, Mode=3 (client)
  packetBuffer[1] = 0;            // Stratum
  packetBuffer[2] = 6;            // Poll interval
  packetBuffer[3] = 0xEC;         // Precision
  
  // Marca tempo locale PRIMA di inviare (per calcolare RTT)
  unsigned long t1_millis = millis();
  
  // Invia il pacchetto
  udp.beginPacket(ntp_server, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  
  // Aspetta risposta (timeout 3 secondi)
  int timeout = 3000;
  while (!udp.parsePacket() && timeout > 0) {
    delay(10);
    timeout -= 10;
  }
  
  if (timeout <= 0) {
    Serial.println("NTP timeout");
    udp.stop();
    return 0.0;
  }
  
  // Marca tempo locale DOPO aver ricevuto (per calcolare RTT)
  unsigned long t4_millis = millis();
  
  // Leggi la risposta
  udp.read(packetBuffer, NTP_PACKET_SIZE);
  udp.stop();
  
  // Estrai Transmit Timestamp (T3) dal server - bytes 40-47
  // Questo è il momento in cui il server ha inviato la risposta
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  unsigned long t3_seconds = highWord << 16 | lowWord;
  
  // Frazione di secondo di T3
  unsigned long t3_fraction = word(packetBuffer[44], packetBuffer[45]);
  t3_fraction = (t3_fraction << 16) | word(packetBuffer[46], packetBuffer[47]);
  double t3_frac_seconds = (double)t3_fraction / 4294967296.0; // 2^32
  
  // Calcola Round-Trip Time in secondi (tempo locale, non richiede sincronizzazione)
  double rtt_seconds = (double)(t4_millis - t1_millis) / 1000.0;
  
  // Converti da NTP epoch (1900) a Unix epoch (1970)
  const unsigned long seventyYears = 2208988800UL;
  
  // L'ora corrente è T3 + metà del round-trip time
  // (T3 è quando il server ha inviato, aggiungiamo il tempo di trasmissione stimato)
  double epoch = (double)(t3_seconds - seventyYears) + t3_frac_seconds + (rtt_seconds / 2.0);
  
  // Debug info
  Serial.printf("NTP sync: RTT=%.1f ms, epoch=%.3f\n", rtt_seconds * 1000.0, epoch);
  
  return epoch;
}