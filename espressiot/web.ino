//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Web Server with Options and Plotter
//

#ifdef ENABLE_HTTP

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "html.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void handleConfig() {
  String message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  if(tuning) {
    message += "<h1> PID TUNING MODE RUNNING !</h1>";
    message += "<a href=\"/tuningstats\"><button>Stats</button></a><br/>\n";
    message += "<hr/>\n";
  }
  message += "<form action=\"set_config\">\nTarget Temperature:<br>\n";
  message += "<input type=\"text\" name=\"tset\" value=\"" + String(gTargetTemp) +"\"><br/><br/>\n";
  message += "<form action=\"set_config\">\nThreshold for adaptive PID:<br>\n";
  message += "<input type=\"text\" name=\"tband\" value=\"" + String(gOvershoot) +"\"><br/><br/>\n";
  message += "normal PID:<br>\n P <input type=\"text\" name=\"pgain\" value=\"" + String(gP) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"igain\" value=\"" + String(gI) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"dgain\" value=\"" + String(gD) + "\"><br><br>\n";
  message += "adaptive PID:<br>\n P <input type=\"text\" name=\"apgain\" value=\"" + String(gaP) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"aigain\" value=\"" + String(gaI) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"adgain\" value=\"" + String(gaD) + "\"><br><br>\n";
  message += "<input type=\"submit\" value=\"Submit\">\n</form>";
  message += "<hr/>";
  message += "<a href=\"./loadconf\"><button>Load Config</button></a><br/>\n";
  message += "<a href=\"./saveconf\"><button>Save Config</button></a><br/>\n";
  message += "<a href=\"./resetconf\"><button>Reset Config to Default</button></a><br/>\n";
  message += "<a href=\"./update\"><button>Update Firmware</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<form action=\"set_tuning\">\nTuning Threshold (\°C):<br>\n";
  message += "<input type=\"text\" name=\"tunethres\" value=\"" + String(aTuneThres) +"\"><br>\n";
  message += "Tuning Power (heater)<br>\n";
  message += "<input type=\"text\" name=\"tunestep\" value=\"" + String(aTuneStep) + "\"><br><br>\n";
  message += "<input type=\"submit\" value=\"Submit\">\n</form><br/>";
  if(!tuning) message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Start PID Tuning Mode</button></a><br/>\n";
  else message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/\"><button>Back</button></a><br/>\n";
  server.send(200, "text/html", message);
}

void handleTuningStats() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"5\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  message += "<h1> PID TUNING STATS </h1>";
  message += "Total power-on-cycles: " + String(tune_count)+"<br/>\n";
  message += "Time elapsed: " + String(tune_time-tune_start)+" ms<br/>\n";
  message += "Average Period: " + String( float(tune_time-tune_start)/tune_count)+" ms<br/>\n";
  message += "Upper Average: " + String(AvgUpperT/UpperCnt)+" °C<br/>\n";
  message += "Lower Average: " + String(AvgLowerT/LowerCnt)+" °C<br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  server.send(200, "text/html", message);
}

void handleSetConfig() {
  // String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  // for ( uint8_t i = 0; i < server.args(); i++ ) {
  //   if(server.argName(i)=="tset") {
  //         message += "new tset: " + server.arg ( i ) + "<br/>\n";
  //         gTargetTemp = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="tband") {
  //         message += "new tset: " + server.arg ( i ) + "<br/>\n";
  //         gOvershoot = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="pgain") {
  //         message += "new pgain: " + server.arg ( i ) + "<br/>\n";
  //         gP = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="igain") {
  //         message += "new igain: " + server.arg ( i ) + "<br/>\n";
  //         gI = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="dgain") {
  //         message += "new pgain: " + server.arg ( i ) + "<br/>\n";
  //         gD = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="apgain") {
  //         message += "new pgain: " + server.arg ( i ) + "<br/>\n";
  //         gaP = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="aigain") {
  //         message += "new igain: " + server.arg ( i ) + "<br/>\n";
  //         gaI = ( (server.arg(i)).toFloat() );
  //   }
  //   else if(server.argName(i)=="adgain") {
  //         message += "new pgain: " + server.arg ( i ) + "<br/>\n";
  //         gaD = ( (server.arg(i)).toFloat() );
  //   }
    
  //}
  StaticJsonBuffer<200> newBuffer;
  JsonObject& newjson = newBuffer.parseObject(server.arg("plain"));
  String message = newjson.get<unsigned int>("targetSlider") + " ";
  message += newjson.get<double>("thresholdSlider");
  server.send(200, "text/html", message);
  
}

void handleSetTuning() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if(server.argName(i)=="tunethres") {
          message += "new tuning threshold: " + server.arg ( i ) + "<br/>\n";
          aTuneThres = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="tunestep") {
          message += "new tuning power: " + server.arg ( i ) + "<br/>\n";
          aTuneStep = ( (server.arg(i)).toFloat() );
    }
  }
  server.send(200, "text/html", message);
  
}

void handleLoadConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if(loadConfig()) message += "<h1>Configuration loaded !</h1>\n";
  else message += "<h1>Error loading configuration !</h1>\n";
  server.send(200, "text/html", message);
}

void handleSaveConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if(saveConfig()) message += "<h1>Configuration saved !</h1>\n";
  else message += "<h1>Error saving configuration !</h1>\n";
  server.send(200, "text/html", message);
}

void handleResetConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  resetConfig();
  message += "<h1>Configuration set to default !</h1>\n";
  server.send(200, "text/html", message);
}



void handleTuningMode() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if(!tuning) {
    tuning_on();
    message += "<h1> Started ! </h1>";
  }
  else {
    message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    tuning_off();
    message += "<h1> Finished and new parameters calculated ! </h1>";
    message += "Total power-on-cycles: " + String(tune_count)+"<br/>\n";
    message += "Average Period: " + String( float(tune_time-tune_start)/tune_count)+" ms<br/>\n";
    message += "Average Peak-To-Peak Temperature: " + String((AvgUpperT/UpperCnt)-(AvgLowerT/LowerCnt))+" °C<br/>\n";
    message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  }
  
  server.send(200, "text/html", message);
}

void handleTurnOnHeater() {
  String message = "Heating On";
  poweroffMode = false;
  server.send(200, "text/html", message);
}
void handleTurnOffHeater() {
  String message = "Power Off";
  poweroffMode = true;
  server.send(200, "text/html", message);
}

void handleToggleSteam(){
  String message;
  if(steaming){
    steaming = false;
    message = "Steaming Mode Off";
  }else{
    steaming = true;
    message = "Steaming Mode On";
  }
  server.send(200, "text/html", message);
}

void handleSendState(){
  String currentTemp;
  if(fahrenheitOn){
    currentTemp = (String)round(1.8*gInputTemp+32);
  }else{
    currentTemp = String(gInputTemp);
  }
  String message = "{\"current\":" + currentTemp;
  message += ", \"target\": " + String(gTargetTemp);
  message += ", \"threshold\": " + String(gOvershoot);
  message += ", \"normalP\": " + String(gP);
  message += ", \"normalI\": " + String(gI);
  message += ", \"normalD\": " + String(gD);
  message += ", \"adaptiveP\": " + String(gaP);
  message += ", \"adaptiveI\": " + String(gaI);
  message += ", \"adaptiveD\": " + String(gaD);
  message += ", \"isAdaptivePID\": " + String(!osmode);
  message += ", \"output\": " + String(gOutputPwr/10) + "}";
  server.send(200, "application/json", message);
}

void setupWebSrv() {
 
  httpUpdater.setup(&server);
  Serial.print("Updater running !");
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/loadconf", handleLoadConfig);
  server.on("/saveconf", handleSaveConfig);
  server.on("/resetconf", handleResetConfig);
  server.on("/set_config", handleSetConfig);
  server.on("/tuningmode", handleTuningMode);
  server.on("/tuningstats", handleTuningStats);
  server.on("/set_tuning", handleSetTuning);
  server.on("/heateron", handleTurnOnHeater);
  server.on("/heateroff", handleTurnOffHeater);
  server.on("/togglesteam", handleToggleSteam);
  server.on("/state", handleSendState);
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println("HTTP server started");
}

void loopWebSrv() {
  server.handleClient();
}

#endif

