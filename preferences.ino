#include "FS.h"
#include "SPIFFS.h"
#include <Preferences.h> //https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/preferences.html

Preferences preferences;
String preferences_namespace="parametri"; //uso solo un namespace...

// key deve avere max 15 caratteri !
boolean preferences_set_string(String key, String value){
  //Serial.println("esecuzione: preferences_set_string("+key+", "+value+")"+ "preferences_namespace="+preferences_namespace);
  
  //da stringa a charArray
  int str_len = preferences_namespace.length() + 1; 
  char preferences_namespace_ca[str_len]; //ca sta per charArray
  preferences_namespace.toCharArray(preferences_namespace_ca, str_len);

  //da stringa a charArray
  int str_len2 = key.length() + 1; 
  char key_ca[str_len2];//era   char key_ca[str_len];
  key.toCharArray(key_ca, str_len2);

  
  if(preferences.begin(preferences_namespace_ca)){
  preferences.putString(key_ca, value);
  String x = preferences.getString(key_ca);
  preferences.end();
  return x.equals(value);
  }
  return false;
}

String preferences_get_string(String key){
  
  //da stringa a charArray
  int str_len = preferences_namespace.length() + 1; 
  char preferences_namespace_ca[str_len];
  preferences_namespace.toCharArray(preferences_namespace_ca, str_len);

  //da stringa a charArray
  int str_len2 = key.length() + 1; 
  char key_ca[str_len2];  //era char key_ca[str_len];
  key.toCharArray(key_ca, str_len2);
  
  if(preferences.begin(preferences_namespace_ca)){
    String x = preferences.getString(key_ca);
    preferences.end();
    return x;
  }
  return "";
}
