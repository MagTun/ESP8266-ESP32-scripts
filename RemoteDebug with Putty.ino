/*
 * RemoteDebug for ESP8266 - NodeMCU
 * 
 * How to use on Windows 10
 * 1) Download PuTTy (open source) from https://www.chiark.greenend.org.uk/~sgtatham/putty/  and install it
 * 2) Install the RemoteDebug library:
 *      Open this link https://github.com/JoaoLopesF/RemoteDebug
 *      Click on "Clone or Download", then click on "Download as a zip"
 *      Unzip the file and move the unzipped folder to C:\Users\<your_username>\Documents\Arduino\libraries
 * 3) Prepare the sketch :
 *    Open Arduino IDE, copy this code
 *    Set your ssid (name of your wireless connection: to get it, click on the wifi symbol on the rigth part of the toobar) and the wifi password
 *    Save the sketch and verify it by clicking the checkmark in Arduino IDE
 * 4) Upload the sketch
 *    Connect your ESP8266 to your computer, set the right Board configuration, the right port
 *    click on Upload
 * 5) Getting the remote debug in Putty
 *    get the local IP address of your ESP8266: it should look like this 192.168.1.16 (if you don't know it connect to your router configuration page)
 *    open Putty, 
 *    enter your ESP8266 IP address under "Host Name (or IP address)" 
 *    enter  23 under port (the port number is set by  MDNS.addService("telnet", "tcp", 23); )
 *    you can save this configuration by click on "Save"
 *    then click Open
 * 6) Miscellinous
 *    the debug level are errors  (show only errors), warning (show only errors and warning), info (show only errors, warning and info), debug (show debug + the 3 previous level ― everything without verbose), verbose (show everything)
 *    Changing Debug level in Putty : by defaut the level is Debug, you can change the level in Putty by pressing V(for verbose), D(for debug)... 
 *    
 * 
 * 
 * 
 * 
 */ 

// for Wifi 
#include <ESP8266WiFi.h>

// for mDNS
#include <DNSServer.h>
#include <ESP8266mDNS.h>


// Remote debug over telnet - not recommended for production, only for development
#include "RemoteDebug.h"  

RemoteDebug Debug;

// SSID and password
const char* ssid = "xxxxx";
const char* password = "xxxxx";

// Host mDNS

#define HOST_NAME "remotedebug"


void setup() {
    // Enable serial
    Serial.begin(115200);
    
    // get Serial.print in serial monitor nn
    setup_wifi();   
    setup_remoteDebug();  
}

void loop()
{
  // simple debug  : other level are rdebugIln() , rdebugWln()  , rdebugEln() ,  rdebugA
  rdebugDln("This will print your text"); 
  rdebugVln("To see verbose message in Putty, you should set the level debug to verbose by pressing V");  

  //debug with variable: the flag %.... will be replaced by the var  : 
  //for the right flag see https://en.wikipedia.org/wiki/Printf_format_string#Type_field
  //if you don't declare the right flag, Putty will either write something weird or "freeze" for 10sec when attempting to connect and then you will get " Network error: Connection timed out "

  const char*  char_var = "sample var";
  rdebugDln("This will print the char_var: %s", char_var); //%s = will replace by variable
  rdebugDln("Declare ssid at the begining of the script: %s", ssid); //%s = ssid was declared as char* 
  rdebugDln("If use the wrong flag, putty print a weird char_var : %c", char_var); //%c = will replace by variable  ―― putty doesn't freeze but print a weird output

  bool boolean_var =true;
  rdebugDln("This will print your var: %d", boolean_var); //

  int int_var = 5;
  rdebugDln("This will print your var: %d", int_var); //
  rdebugDln("This will print your var: %u", int_var); // doesn't seems to be a problem


  // Should work with serial.println if enable in setup_remoteDebug() (cf. Debug.setSerialEnabled(true) ), but couldn't make it appear in Putty
   Serial.println("This is a Serial.print()"); // couldn't make it work

  // Send the rebug to Putty 
  Debug.handle();
  delay(5000);
  // Wait before running the next loop for the background tasks to finish (eg: wifi)
  yield();

}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connexion OK ");
  Serial.print("=> Addresse IP : ");
  Serial.print(WiFi.localIP());
}

void setup_remoteDebug() {
    // Register host name in WiFi and mDNS
    String hostNameWifi = HOST_NAME;
    hostNameWifi.concat(".local");
    WiFi.hostname(hostNameWifi);


    // start mDNS
    if (MDNS.begin(HOST_NAME)) {
        Serial.print("* MDNS responder started. Hostname -> ");
        Serial.println(HOST_NAME);
    }

    MDNS.addService("telnet", "tcp", 23);


    // Initialize the telnet server of RemoteDebug
    Debug.setSerialEnabled(true); // If we want Serial.print() in Putty

    Debug.begin(HOST_NAME); // Initiaze the telnet server

    Debug.setResetCmdEnabled(true); // Enable the reset command

    //Debug.showTime(true); // To show time

    // Debug.showProfiler(true); // To show profiler - time between messages of Debug
                              // Good to "begin ...." and "end ...." messages

    // This sample (serial -> educattional use only, not need in production)
    Serial.println("* Arduino RemoteDebug Library");
    Serial.println("*");
    Serial.print("* WiFI connected. IP address: ");
    Serial.println(WiFi.localIP());
}

