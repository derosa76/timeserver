
//String timestamp(){
//  return String(time_object_gps.getYear())+"-"+format_int(time_object_gps.getMonth(),2)+"-"+format_int(time_object_gps.getDay(),2)+" "+format_int(time_object_gps.getHour(true),2)+":"+format_int(time_object_gps.getMinute(),2)+":"+format_int(time_object_gps.getSecond(),2); 
//}

// String timestamp(){
//   // Selezione sorgente temporale migliore
//   ESP32Time* time_source = nullptr;
  
//   if (gpsTimeIsSet) {
//     time_source = &time_object_gps;
//   }
//   else if (ntp_time_is_set) {
//     time_source = &time_object_ntp;
//   }
  
//   // Se nessuna fonte disponibile
//   if (time_source == nullptr) {
//     return "NO-TIME";
//   }
  
//   // Usa la fonte selezionata
//   return String(time_source->getYear()) + "-" + 
//          format_int(time_source->getMonth(), 2) + "-" + 
//          format_int(time_source->getDay(), 2) + " " + 
//          format_int(time_source->getHour(true), 2) + ":" + 
//          format_int(time_source->getMinute(), 2) + ":" + 
//          format_int(time_source->getSecond(), 2);
// }


String timestamp(){
  // Selezione sorgente temporale migliore
  ESP32Time* time_source = nullptr;
  
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
  return "E:" + String(epoch) + " " +
         String(year) + "-" + 
         format_int(month+1, 2) + "-" + 
         format_int(day, 2) + " " + 
         format_int(time_source->getHour(true), 2) + ":" + 
         format_int(time_source->getMinute(), 2) + ":" + 
         format_int(time_source->getSecond(), 2);
}