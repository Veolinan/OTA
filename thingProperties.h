#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[] = "12216e77-215c-4004-ade4-7913030e8245";

const char SSID[] = SECRET_SSID;  // Network SSID
const char PASS[] = SECRET_OPTIONAL_PASS;  // Network password
const char DEVICE_KEY[] = SECRET_DEVICE_KEY;  // Device key

void onBulbChange();  // Forward declaration for the function

CloudLight bulb;  // Define the CloudLight variable

void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(bulb, READWRITE, ON_CHANGE, onBulbChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
