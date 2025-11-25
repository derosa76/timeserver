
//faccio i conti come se fossimo GMT
//ESP32Time time_object_calcoli(0);

unsigned long fine_ora_legale(int year){
  ESP32Time time_object_calcoli(0);
  time_object_calcoli.setTime(0, 0, 1, 30, 10, year); //parto dalle ore 1 GMT dell'ultimo gg di ottobre (che con l'ora legale in Italia sono le 3)
  Serial.println("calcolo fine_ora_legale iterazioni:");
  Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
  while (!(time_object_calcoli.getDayofWeek()==0)) { //domenica è 0
    time_object_calcoli.setTime(time_object_calcoli.getEpoch()-86400);
    Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
  }
  return time_object_calcoli.getEpoch();
}

unsigned long inizio_ora_legale(int year){
  ESP32Time time_object_calcoli(0);
  time_object_calcoli.setTime(0, 0, 1, 31, 3, year); //parto dalle ore 1 GMT dell'ultimo gg di marzo (ora solare, sono le 2 effettive Italia)
  Serial.println("calcolo inizio_ora_legale iterazioni:");
  Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
  while (!(time_object_calcoli.getDayofWeek()==0)) { //domenica è 0
    time_object_calcoli.setTime(time_object_calcoli.getEpoch()-86400);
    Serial.println(String(time_object_calcoli.getEpoch())+":"+String(time_object_calcoli.getDayofWeek()));
  }
  return time_object_calcoli.getEpoch();
}
