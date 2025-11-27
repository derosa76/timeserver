
String timestamp(){
  // Selezione sorgente temporale migliore
  IndependentTime* time_source = nullptr;
  
  if (gpsTimeIsSet) {
    time_source = &time_object_gps;
  }
  else if (ntp_time_is_set) {
    time_source = &time_object_ntp;
  }
  
  // Se nessuna fonte disponibile
  if (time_source == nullptr) {
    return "NO-TIME";
  }
  
  // DEBUG: vedi cosa legge
  unsigned long epoch = time_source->getEpoch();
  int year = time_source->getYear();
  int month = time_source->getMonth();
  int day = time_source->getDay();
  
  // Usa la fonte selezionata con DEBUG
  return timestamp(time_source);
}


/*
 * Genera un timestamp testuale da un oggetto IndependentTime
 * 
 * Parametri:
 *   t - oggetto IndependentTime
 *   
 * Ritorna:
 *   String formato "YYYY-MM-DD HH:MM:SS.mmm"
 *   Esempio: "2024-12-06 18:45:32.123"
 */
String timestamp(IndependentTime* t) {
  char buffer[24];
  
  // Millisecondi dall'oggetto tempo
  unsigned int millis_part = t->getMillis();
  
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03u",
          t->getYear(),
          t->getMonth() + 1,
          t->getDay(),
          t->getHour(true),
          t->getMinute(),
          t->getSecond(),
          millis_part);
  
  return String(buffer);
}