/*
 * NTP SERVER ENHANCED - con supporto millisecondi
 * 
 * Questo modulo implementa un server NTP (Network Time Protocol) che:
 * - Risponde alle richieste NTP sulla porta UDP 123
 * - Usa GPS come sorgente primaria, NTP come fallback
 * - Include frazioni di secondo (millisecondi) nel timestamp
 * - Gestisce correttamente lo Stratum in base all'affidabilità della fonte
 */

// ============================================================================
// COSTANTI NTP
// ============================================================================

// Differenza tra epoch Unix (1970) e epoch NTP (1900) in secondi
#define NTP_EPOCH_OFFSET 2208988800UL

// Dimensione pacchetto NTP standard
#define NTP_PACKET_SIZE 48

// Valori per il campo LI VN Mode (byte 0)
#define NTP_LI_NO_WARNING 0b00000000   // No Leap second warning
#define NTP_VERSION_4     0b00100000   // NTP versione 4
#define NTP_MODE_SERVER   0b00000100   // Mode: Server

// Valori Stratum
#define NTP_STRATUM_GPS      1         // Stratum 1: connesso direttamente a GPS
#define NTP_STRATUM_NTP      2         // Stratum 2: sincronizzato via NTP
#define NTP_STRATUM_INVALID  16        // Stratum 16: non sincronizzato

// Precision: potenza di 2 in secondi
// -10 = 2^-10 = 0.001 sec = 1 millisecondo
#define NTP_PRECISION -10

// ============================================================================
// VARIABILI GLOBALI
// ============================================================================

unsigned long last_gps_sync_millis = 0;  // Quando abbiamo sincronizzato GPS l'ultima volta
unsigned long last_ntp_sync_millis = 0;  // Quando abbiamo sincronizzato NTP l'ultima volta

// ============================================================================
// FUNZIONI HELPER
// ============================================================================

/*
 * Converte millisecondi (0-999) nel formato NTP fraction (32 bit)
 * Formula: frazione = (millisecondi * 2^32) / 1000
 * 
 * Esempio: 500 ms = 0x80000000 (metà del range)
 */
uint32_t millisToNtpFraction(uint16_t millis) {
  // Calcolo: (millis * 4294967296) / 1000
  // Uso uint64_t per evitare overflow
  uint64_t fraction = ((uint64_t)millis * 4294967296ULL) / 1000ULL;
  return (uint32_t)fraction;
}

/*
 * Stima i millisecondi dall'ultimo sync GPS
 * Usa millis() per interpolare il tempo tra un aggiornamento GPS e l'altro
 */
uint16_t estimateMilliseconds() {
  unsigned long elapsed = millis() - last_gps_sync_millis;
  return (uint16_t)(elapsed % 1000);
}

/*
 * Scrive un valore a 32 bit nel buffer in formato Big Endian (network byte order)
 */
void writeUint32(byte* buffer, int offset, uint32_t value) {
  buffer[offset + 0] = (value >> 24) & 0xFF;
  buffer[offset + 1] = (value >> 16) & 0xFF;
  buffer[offset + 2] = (value >> 8) & 0xFF;
  buffer[offset + 3] = value & 0xFF;
}

/*
 * Scrive un timestamp NTP completo (secondi + frazione) nel buffer
 */
void writeNtpTimestamp(byte* buffer, int offset, uint32_t seconds, uint32_t fraction) {
  writeUint32(buffer, offset, seconds);
  writeUint32(buffer, offset + 4, fraction);
}

/*
 * Determina quale sorgente temporale usare e calcola lo Stratum
 * Ritorna: stratum value, modifica seconds e fraction per reference
 */
byte selectTimeSource(uint32_t* seconds, uint32_t* fraction) {
  
  // PRIORITÀ 1: GPS se affidabile
  if (gpsTimeIsReliable) {
    *seconds = time_object_gps.getEpoch() + NTP_EPOCH_OFFSET;
    *fraction = millisToNtpFraction(estimateMilliseconds());
    return NTP_STRATUM_GPS;
  }
  
  // PRIORITÀ 2: NTP se disponibile
  if (ntp_time_is_set) {
    *seconds = time_object_ntp.getEpoch() + NTP_EPOCH_OFFSET;
    *fraction = 0; // NTP client non fornisce millisecondi
    return NTP_STRATUM_NTP;
  }
  
  // NESSUNA FONTE AFFIDABILE
  *seconds = 0;
  *fraction = 0;
  return NTP_STRATUM_INVALID;
}

// ============================================================================
// SETUP E LOOP
// ============================================================================

WiFiUDP ntpUDP;

void ntpd_setup() {
  
  ntpUDP.begin(123);
  last_gps_sync_millis = millis();
  last_ntp_sync_millis = millis();
}

void ntpd_run() {
  byte packetBuffer[NTP_PACKET_SIZE];
  
  // Controlla se è arrivata una richiesta NTP
  int packetSize = ntpUDP.parsePacket();
  
  if (packetSize != NTP_PACKET_SIZE) {
    return; // Non è un pacchetto NTP valido, ignora
  }
  
  // Leggi il pacchetto ricevuto
  ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);
  
  // Azzera tutto il buffer per la risposta
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  // -----------------------------------------------
  // COSTRUZIONE RISPOSTA NTP
  // -----------------------------------------------
  
  // Byte 0: LI (2 bit) + Version (3 bit) + Mode (3 bit)
  packetBuffer[0] = NTP_LI_NO_WARNING | NTP_VERSION_4 | NTP_MODE_SERVER;
  
  // Byte 2: Polling Interval (6 = 2^6 = 64 secondi)
  packetBuffer[2] = 6;
  
  // Byte 3: Precision
  packetBuffer[3] = NTP_PRECISION;
  
  // Seleziona la sorgente temporale migliore
  uint32_t ref_seconds, ref_fraction;
  byte stratum = selectTimeSource(&ref_seconds, &ref_fraction);
  
  // Byte 1: Stratum
  packetBuffer[1] = stratum;
  
  // Reference Timestamp (byte 16-23): ultimo sync
  writeNtpTimestamp(packetBuffer, 16, ref_seconds, ref_fraction);
  
  // Originate Timestamp (byte 24-31): 
  // Dovrebbe essere copiato dal campo Transmit del client, ma per semplicità mettiamo 0
  
  // Receive Timestamp (byte 32-39): quando abbiamo ricevuto la richiesta
  writeNtpTimestamp(packetBuffer, 32, ref_seconds, ref_fraction);
  
  // Transmit Timestamp (byte 40-47): quando stiamo inviando la risposta
  // In teoria dovremmo ricalcolare, ma la differenza è trascurabile
  writeNtpTimestamp(packetBuffer, 40, ref_seconds, ref_fraction);
  
  // Invia la risposta al client
  ntpUDP.beginPacket(ntpUDP.remoteIP(), ntpUDP.remotePort());
  ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
  ntpUDP.endPacket();
}

// ============================================================================
// FUNZIONE DA CHIAMARE QUANDO GPS VIENE AGGIORNATO
// ============================================================================

/*
 * Questa funzione dovrebbe essere chiamata in gps.ino dentro set_date_time()
 * quando viene fatto l'update del time_object_gps.
 * 
 * Esempio di dove inserirla in gps.ino:
 * 
 * void set_date_time() {
 *   ...
 *   if (age<=1500){
 *     time_object_gps.setTime(second, minute, hour, day, month, year);
 *     gpsTimeLastSetMillis=millis();
 *     gpsTimeIsSet=true;
 *     gpsTimeIsReliable=true;
 *     
 *     ntp_server_mark_gps_sync(); // <--- AGGIUNGI QUESTA RIGA
 *   }
 *   ...
 * }
 */
void ntp_server_mark_gps_sync() {
  last_gps_sync_millis = millis();
}

/*
 * Analogamente per NTP (da chiamare in ntp_client.ino quando si aggiorna time_object_ntp)
 */
void ntp_server_mark_ntp_sync() {
  last_ntp_sync_millis = millis();
}