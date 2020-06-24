#include <M5Stack.h>
#include <BLEDevice.h>

const char *uuid = "0000fd6f-0000-1000-8000-00805f9b34fb";

void string2char(std::string str, char buffer[])
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    byte nib1 = (str[i] >> 4) & 0x0F;
    byte nib2 = (str[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[str.length() * 2] = '\0';
}

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    if (advertisedDevice.haveServiceUUID())
    {
      if (strncmp(advertisedDevice.getServiceUUID().toString().c_str(), uuid, strlen(uuid)) == 0)
      {
        int rssi = advertisedDevice.getRSSI();
        std::string data = advertisedDevice.getServiceData();
        if (data.length() == 20)
        {
          char rpi[32 + 1];
          string2char(data.substr(0, 16), rpi);
          Serial.printf("RSSI: %d Address: %s Rolling Proximity Identifier: %s\n",
                        rssi,
                        advertisedDevice.getAddress().toString().c_str(),
                        rpi);
        }
      }
    }
  }
};

void setup()
{
  M5.begin();
  BLEDevice::init("");
}

void loop()
{
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  Serial.println("Scan Start");
  pBLEScan->start(1, false);
  Serial.println("Scan End");

  M5.update();
  delay(1000);
}