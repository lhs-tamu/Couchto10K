//Connects esp32 to Both Polar H10 and Arduino Nano 33 ble
//Does not diplay anything other than sensor data on com port to allow for python script to save data to a .csv file




#include "BLEDevice.h"
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID(BLEUUID((uint16_t)0x180D));
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID(BLEUUID((uint16_t)0x2A37));


static BLEUUID serviceUUID1("19b10000-e8f2-537e-4f6c-d104768a1214");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID1("19b10001-e8f2-537e-4f6c-d104768a1214");

static boolean doConnect = false;
static boolean connected = false;
static boolean doConnect1 = false;
static boolean connected1 = false;
static boolean doScan = false;
static boolean doScan1 = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLERemoteCharacteristic* pRemoteCharacteristic1;
static BLEAdvertisedDevice* myDevice;
static BLEAdvertisedDevice* myDevice1;
int HR;
float RR;
int steps;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    //Serial.print("Heart Rate: ");
    for (int i = 0; i < 6; i++) {
    //Serial.print(pData[i]);
        //Serial.print(" ");
  }
  HR=pData[1];
 //Serial.print(pData[1]);
 //Serial.println();
 uint16_t RR1 =(pData[3]*256+pData[2]);
 uint16_t RR2 =(pData[5]*256+pData[4]);
 //for (int b = 15; b >= 0; b--)
 // {
  //Serial.print(bitRead(RR1,b));  }
 //Serial.print(RR1); 
 //Serial.print(" ");
 //Serial.print(RR2); 
 //Serial.println();

 double RR1s=RR1/1024.0;
 RR=RR1s;
 //double RR2s=RR2/1024.0;
 //Serial.print(RR1s);
 //Serial.print(" ");
 //Serial.println();
 //Serial.print(RR2s);

 //RRA[next]=RR1s;

 
  
  //for (int i = 0; i <= next; i++) {
    //Serial.print(RRA[i]);
    //Serial.print(" ");
 // }
  //Serial.println();

  // next+=1;
}

static void notifyCallback1(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    steps = pData[0] | (pData[1] << 8);
  //Serial.print(steps);
  //Serial.println();
  }

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }
 

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    //connected1= false;
    //Serial.println("onDisconnect");
  }
  
};




bool connectToServer() {
    //Serial.print("Forming a connection to ");
    //Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    //Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    //Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      //Serial.print("Failed to find our service UUID: ");
      //Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      //Serial.print("Failed to find our characteristic UUID: ");
      //Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial.println(" - Found our characteristic");

    

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    //pClient->disconnect();
}

bool connectToServer1() {
    //Serial.print("Forming a connection to ");
    //Serial.println(myDevice1->getAddress().toString().c_str());
    
    BLEClient*  pClient1  = BLEDevice::createClient();
    //Serial.println(" - Created client1");

    pClient1->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient1->connect(myDevice1);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    //Serial.println(" - Connected to server1");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService1 = pClient1->getService(serviceUUID1);
    if (pRemoteService1 == nullptr) {
      //Serial.print("Failed to find our service UUID1: ");
      //Serial.println(serviceUUID.toString().c_str());
      pClient1->disconnect();
      return false;
    }
    //Serial.println(" - Found our service1");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic1 = pRemoteService1->getCharacteristic(charUUID1);
    if (pRemoteCharacteristic1 == nullptr) {
      //Serial.print("Failed to find our characteristic UUID1: ");
      //Serial.println(charUUID.toString().c_str());
      pClient1->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic1");


    if(pRemoteCharacteristic1->canNotify())
      pRemoteCharacteristic1->registerForNotify(notifyCallback1);

    connected1 = true;
    //pClient->disconnect();
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      //BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      //doScan = true;

    } // Found our server
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID1)) {

      //BLEDevice::getScan()->stop();
      myDevice1 = new BLEAdvertisedDevice(advertisedDevice);
      doConnect1 = true;
      //doScan1 = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  //Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //pBLEScan->setInterval(1349);
  //pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10);
  //pBLEScan->start(5, false);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      //Serial.println("We are now connected to the BLE Server.");
    } else {
      //Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
  if (doConnect1 == true) {
    if (connectToServer1()) {
      //Serial.println("We are now connected to the BLE Server.");
    } else {
      
      //Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect1 = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    //Serial.print("Heart Rate:");
     Serial.print(HR);
     Serial.println();
     //Serial.print(" RR-Interval:");
     Serial.print(RR);
     Serial.println();
     
    }
 if(connected1){
    //Serial.print(" Steps:");
     Serial.print(steps);
 }
    Serial.println();
     

 
  
  
  delay(1000); // Delay a second between loops.
} // End of loop
