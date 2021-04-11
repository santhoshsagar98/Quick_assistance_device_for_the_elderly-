#include <SPI.h>
#include <WiFi101.h>
#include <WiFiSSLClient.h>
#include <TembooSSL.h>
#include "utility/TembooGPIO.h"
#include "TembooAccount.h" // Contains Temboo account information

WiFiSSLClient client;
// We limit this so you won't use all of your Temboo calls while testing
// The number of times to trigger the action if the condition is met

int maxCalls = 10;

// The number of times this Choreo has been run so far in this sketch
int calls = 0;
//Buzzer pin 
int buzzer = 1;

// Declaring sensor configs
TembooGPIOConfig tmb_buttonConfig;

// Declaring TembooSensors
TembooSensor tmb_button;

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(2000);
  
  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(3000);
  }

  // Initialize sensors and configs
  tembooDigitalGPIOInit(&tmb_buttonConfig, &tmb_button, 2, LOW, INPUT);
  pinMode(buzzer,OUTPUT);
  Serial.println("Setup complete.\n");
}

void loop() {
  int sensorValue = tmb_button.read(&tmb_buttonConfig);
  Serial.println("Sensor: " + String(sensorValue));
  if (sensorValue == HIGH) {
    if (calls < maxCalls) {
      Serial.println("\nTriggered! Calling Publish Choreo...");
      tone(buzzer,500,2000);
      runPublish(sensorValue);
      calls++;
    } else {
      Serial.println("\nTriggered! Skipping to save Temboo calls. Adjust maxCalls as required.");
    }
  }
  delay(250);
}

void runPublish(int sensorValue) {
  TembooChoreoSSL PublishChoreo(client);

  // Set Temboo account credentials
  PublishChoreo.setAccountName(TEMBOO_ACCOUNT);
  PublishChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  PublishChoreo.setAppKey(TEMBOO_APP_KEY);
  PublishChoreo.setDeviceType(TEMBOO_DEVICE_TYPE);
  // Set Choreo inputs
 String MessageValue = "EMERGENCY PLEASE HELP Apt 109 Porur Square Apt, Chettiyar Agaram Road , Porur , Chennai -116";
  PublishChoreo.addInput("Message", MessageValue);
StringTopicArnValue = "";
  PublishChoreo.addInput("TopicArn", TopicArnValue);
  String UserRegionValue = "us-west-2";
  PublishChoreo.addInput("UserRegion", UserRegionValue);
  String AWSAccessKeyIdValue = "";
  PublishChoreo.addInput("AWSAccessKeyId", AWSAccessKeyIdValue);
StringAWSSecretKeyIdValue="";
PublishChoreo.addInput("AWSSecretKeyId", AWSSecretKeyIdValue);
String SubjectValue = "Emergency";
  PublishChoreo.addInput("Subject", SubjectValue);
  // Identify the Choreo to run
  PublishChoreo.setChoreo("/Library/Amazon/SNS/Publish");

  // Run the Choreo
  unsigned int returnCode = PublishChoreo.run();

  // Read and print the error message
  while (PublishChoreo.available()) {
    char c = PublishChoreo.read();
    Serial.print(c);
  }
  Serial.println();
  PublishChoreo.close();
}
