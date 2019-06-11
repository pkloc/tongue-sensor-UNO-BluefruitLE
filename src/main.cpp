#include <Arduino.h>
#include <SPI.h>
#include <BLEPeripheral.h>


const int8_t BLE_REQ = 10;
const int8_t BLE_RDY = 2;
const int8_t BLE_RST = 9;

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);


BLEService sensorService("887039ef-52e5-49e6-96ee-4142baaf38ed");
BLEIntCharacteristic sensorCharacteristic("887039ef-52e5-49e6-96ee-4142baaf38ed", BLERead | BLENotify);
BLEIntCharacteristic thresholdCharacteristic("887039ef-52e5-49e6-96ee-4142baaf38ed", BLERead | BLEWrite);
BLEIntCharacteristic ledCharacteristic("887039ef-52e5-49e6-96ee-4142baaf38ed", BLERead | BLEWrite);

const int8_t LED_PIN = 7;
const int8_t SENSOR_PIN = 0;

int lastSensorValue = 0;
int threshold = 10;

void ledOn();
void ledOff();
void handleNewSensorValue(int sensorValue);

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);

    Serial.println("Setup peripheral");

    // set local name for the peripheral and advertise the led service
    blePeripheral.setLocalName("ArduinoSensor");
    blePeripheral.setAdvertisedServiceUuid(sensorService.uuid());

    // add the service and chracteristic as attributes of the peripheral
    blePeripheral.addAttribute(sensorService);
    blePeripheral.addAttribute(sensorCharacteristic);
    blePeripheral.addAttribute(thresholdCharacteristic);
    blePeripheral.addAttribute(ledCharacteristic);

    ledCharacteristic.setValue(0);
    thresholdCharacteristic.setValue(threshold);

    // begin advertising the led service
    blePeripheral.begin();
    Serial.println("Arduino peripheral active");
}

void loop() {
    BLECentral central = blePeripheral.central();   // listen for connections
    
    // if you get a connection
    if(central){
        Serial.print("Connected to central: ");
        Serial.println(central.address());

        while(central.connected()){
            if(thresholdCharacteristic.written()){
                threshold = thresholdCharacteristic.value();
            }
            if(ledCharacteristic.written()){
                if(ledCharacteristic.value() == 1){
                    ledOn();
                }
                if(ledCharacteristic.value() == 0){
                    ledOff();
                }
            }

            int sensorValue = analogRead(SENSOR_PIN);
            if(abs(sensorValue - lastSensorValue) > threshold){
                handleNewSensorValue(sensorValue);
                lastSensorValue = sensorValue;
            }
        }

        Serial.print("Disconnnected from central: ");
        Serial.println(central.address());
    }
}

void ledOn(){
    Serial.println("led on");
    digitalWrite(LED_PIN, HIGH);    
}

void ledOff(){
    Serial.println("led off");
    digitalWrite(LED_PIN, LOW);
}

void handleNewSensorValue(int sensorValue){
    sensorCharacteristic.setValue(sensorValue);
    Serial.print(threshold);
    Serial.print(".");
    Serial.println(sensorValue);
}