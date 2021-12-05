//Initilizes Arduino Nano 33 ble as BLE server and extrapolates step count from onboard accleremter data


#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

//Defines remote service and characteristic
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService stepService(deviceServiceUuid); 
BLEIntCharacteristic stepChar(deviceServiceCharacteristicUuid, BLENotify | BLEWrite);




float threshold = 1.8;
float xval[100] = {0};
float yval[100] = {0};
float zval[100] = {0};
float xavg, yavg, zavg;
int steps, flag, prevstep = 0;


void setup() {
 Serial.begin(115200);

 Serial.println("Started");

 //Initalize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }











 
 //while (!Serial);
 pinMode(LED_BUILTIN, OUTPUT);

  //Initalize BLE server
   if (!BLE.begin()) {
  Serial.println("starting BLE failed!");
  while (1);
    }

  //Define ble name, service, characteristic, and begin advertising  
  BLE.setLocalName("ArduinoNano33");
  BLE.setAdvertisedService(stepService);
  stepService.addCharacteristic(stepChar);
  BLE.addService(stepService);
  BLE.advertise();
}




void loop() {
  







  
 
    //define device we are connected to aka central
    BLEDevice central = BLE.central();
  
//Find and display central adress and turn on led if connected
  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);
   
  }
  //while connected run functions
  while (central.connected()) {
      stepcounter();
      //Not using stepupdate() as we need step value to be sent even if step characteritcs  didnt change
      //stepupdate();
      stepreset();
      
      stepChar.writeValue(steps);
      Serial.print("steps: ");
      Serial.println(steps);

       
      
      delay(500);
       
      }
      
         
      

//if not connected turn led off
digitalWrite(LED_BUILTIN, LOW);

//Serial.print("Disconnected from central: ");
//Serial.println(central.address());
  
  } 
//Extrapolates step count from accelerometer DATA
void stepcounter(){
   float x, y, z;

  

  int acc = 0;
  float totvect[100] = {0};
  float totave[100] = {0};
  float xaccl[100] = {0};
  float yaccl[100] = {0};
  float zaccl[100] = {0};

  
  for (int a = 0; a < 100; a++)
  {
    
      IMU.readAcceleration(x, y, z);
      
      xaccl[a] = x ;
      delay(1);
      yaccl[a] = y ;
      delay(1);
      zaccl[a] = z ;
      delay(1);
      totvect[a] = sqrt(((xaccl[a] - xavg) * (xaccl[a] - xavg)) + ((yaccl[a] - yavg) * (yaccl[a] - yavg)) + ((zval[a] - zavg) * (zval[a] - zavg)));
      totave[a] = (totvect[a] + totvect[a - 1]) / 2 ;
      //Serial.println("totave[a]");
      //Serial.println(totave[a]);
      delay(7);
      if (totave[a] > threshold && flag == 0)
    {
      steps = steps + 1;
      flag = 1;
    }
    else if (totave[a] > threshold && flag == 1)
    {
      // Don't Count
    }
    if (totave[a] < threshold   && flag == 1)
    {
      flag = 0;
    }
    if (steps < 0) {
      steps = 0;
    }
           
    }
   
  }
//Only update step characteristic if steps have changed
  void stepupdate(){
    if(steps!=prevstep){
       Serial.print("steps: ");
       Serial.println(steps);
       stepChar.writeValue(steps);
       prevstep=steps;
      }
    }
//If central writes the int 0 to the step charateristic the steps will reset back to 0
 void stepreset(){
  if (stepChar.written()) {
        if (stepChar.value()==0) {   
           steps=0;
           prevstep=0; 
           Serial.print("steps: ");
           Serial.println(steps);
           //stepChar.writeValue(steps);     
        } 
    }
  }
