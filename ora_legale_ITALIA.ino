
// a partire da un oggetto tempo GMT ESP32Time 
// calcola l'offset dell'ora Italia rispetto a GMT, in secondi.
// (offset = quanti secondi devo aggiungere a GMT)

int time_offset_italia(IndependentTime& t){
  unsigned long ntp_inizio_ora_legale = inizio_ora_legale(t.getYear());
  unsigned long ntp_fine_ora_legale = fine_ora_legale(t.getYear());
  if (t.getEpoch() >= ntp_inizio_ora_legale) {
    if (t.getEpoch() < ntp_fine_ora_legale) {
      return 7200;
    }
  }
  return 3600;
}


unsigned long fine_ora_legale(int year) {
  // Finisce: ultima domenica di ottobre alle 03:00 ora legale italiana (GMT+2)
  // In GMT: 03:00 - 2h = 01:00 GMT

  // Calcola giorni dal 1-gen-1970 al 31 ottobre dell'anno target
  unsigned long days = 0;
  
  // Conta giorni dal 1970 fino all'anno target
  for (int y = 1970; y < year; y++) {
    bool isLeap = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
    days += isLeap ? 366 : 365;
  }
  
  // Aggiungi giorni dei mesi da gennaio a settembre
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30}; // gen-set
  bool isLeap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
  if (isLeap) daysInMonth[1] = 29;
  
  for (int m = 0; m < 9; m++) {
    days += daysInMonth[m];
  }
  
  // Aggiungi 31 giorni di ottobre (arriviamo al 31 ottobre) - CORRETTO!
  days += 31;
  
  // Epoch del 31 ottobre alle 01:00 GMT
  unsigned long epoch = days * 86400UL + 3600UL;
  
  // Calcola giorno della settimana
  // 1-gen-1970 era giovedì (4), formula: (days + 4) % 7
  // 0=dom, 1=lun, 2=mar, 3=mer, 4=gio, 5=ven, 6=sab
  int dayOfWeek = (days + 4) % 7;
  
  // Torna indietro fino a trovare domenica (0)
  while (dayOfWeek != 0) {
    epoch -= 86400UL; // -1 giorno
    days--;
    dayOfWeek = (days + 4) % 7;
  }
  return epoch;
}


unsigned long inizio_ora_legale(int year) {
  //Inizia: ultima domenica di marzo alle 02:00 ora solare italiana (GMT+1)
  //In GMT: 02:00 - 1h = 01:00 GMT

  // Calcola giorni dal 1-gen-1970 al 31 marzo dell'anno target
  unsigned long days = 0;
  
  // Conta giorni dal 1970 fino all'anno target
  for (int y = 1970; y < year; y++) {
    bool isLeap = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
    days += isLeap ? 366 : 365;
  }
  
  // Aggiungi giorni di gennaio e febbraio
  bool isLeap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
  days += 31; // gennaio
  days += isLeap ? 29 : 28; // febbraio
  days += 31; // marzo completo
  
  // Epoch del 31 marzo alle 01:00 GMT
  unsigned long epoch = days * 86400UL + 3600UL;
  
  // Calcola giorno della settimana
  int dayOfWeek = (days + 4) % 7;
  
  // Torna indietro fino a trovare domenica (0)
  while (dayOfWeek != 0) {
    epoch -= 86400UL; // -1 giorno
    days--;
    dayOfWeek = (days + 4) % 7;
  }
  
  return epoch;
}

