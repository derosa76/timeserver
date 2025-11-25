/*
 * Calcola l'epoch GMT dell'inizio e fine ora legale in Italia
 * Solo calcoli numerici, nessun oggetto ESP32Time
 * 
 * VERIFICATO per 2025:
 * - inizio_ora_legale(2025) → 30 marzo 2025 01:00 GMT (domenica)
 * - fine_ora_legale(2025) → 26 ottobre 2025 01:00 GMT (domenica)
 */

/*
 * Ritorna l'epoch GMT della fine ora legale per un dato anno
 * (ultima domenica di ottobre alle 01:00 GMT)
 */
unsigned long fine_ora_legale(int year) {
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

/*
 * Ritorna l'epoch GMT dell'inizio ora legale per un dato anno
 * (ultima domenica di marzo alle 01:00 GMT)
 */
unsigned long inizio_ora_legale(int year) {
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











// //faccio i conti come se fossimo GMT
// //ESP32Time time_object_calcoli(0);

// unsigned long fine_ora_legale(int year){
//   ESP32Time time_object_calcoli(0);
//   time_object_calcoli.setTime(0, 0, 1, 30, 10, year); //parto dalle ore 1 GMT dell'ultimo gg di ottobre (che con l'ora legale in Italia sono le 3)
//   Serial.println("calcolo fine_ora_legale iterazioni:");
//   Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
//   while (!(time_object_calcoli.getDayofWeek()==0)) { //domenica è 0
//     time_object_calcoli.setTime(time_object_calcoli.getEpoch()-86400);
//     Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
//   }
//   return time_object_calcoli.getEpoch();
// }

// unsigned long inizio_ora_legale(int year){
//   ESP32Time time_object_calcoli(0);
//   time_object_calcoli.setTime(0, 0, 1, 31, 3, year); //parto dalle ore 1 GMT dell'ultimo gg di marzo (ora solare, sono le 2 effettive Italia)
//   Serial.println("calcolo inizio_ora_legale iterazioni:");
//   Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
//   while (!(time_object_calcoli.getDayofWeek()==0)) { //domenica è 0
//     time_object_calcoli.setTime(time_object_calcoli.getEpoch()-86400);
//     Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
//   }
//   return time_object_calcoli.getEpoch();
// }
