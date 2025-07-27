

#ifndef Defines_h
#define Defines_h

#include <M5StickCPlus.h>

#define ESP_M5

//!name of service to call (which will always be PTFeeder)
#define MAIN_BLE_CLIENT_SERVICE (char*)"PTFeeder"
//!name of service to call (which will always be PTClicker)
#define M5_BLE_CLIENT_SERVICE (char*)"PTClicker"

//!Defines the name of the service of the server, which for M5 will be PTClicker
#define MAIN_BLE_SERVER_SERVICE_NAME_PTClicker (char*)"PTClicker"
#define MAIN_BLE_SERVER_SERVICE_NAME_PTFeeder (char*)"PTFeeder"
#define MAIN_BLE_SERVER_SERVICE_NAME MAIN_BLE_SERVER_SERVICE_NAME_PTFeeder


#define SERIAL_DEBUG_ERROR true
#define SerialError if (SERIAL_DEBUG_ERROR) Serial
#define SERIAL_DEBUG_LOTS false
#define SerialLots  if (SERIAL_DEBUG_LOTS) Serial
#define SERIAL_DEBUG_DEBUG true
#define SerialDebug  if (SERIAL_DEBUG_DEBUG) Serial
#define SERIAL_DEBUG_INFO false
#define SerialInfo  if (SERIAL_DEBUG_INFO) Serial
#define SERIAL_DEBUG_MINIMAL true
#define SerialMin  if (SERIAL_DEBUG_MINIMAL) Serial
// a temporary debug.. without having to set the above..
#define SERIAL_DEBUG_TEMP true
#define SerialTemp  if (SERIAL_DEBUG_TEMP) Serial
//turn on CALLS to see the methods called in order, etc
#define SERIAL_DEBUG_CALL false
#define SerialCall  if (SERIAL_DEBUG_CALL) Serial


//! start with this one..
#define USE_BLE_SERVER_NETWORKING


#ifdef USE_MQTT_NETWORKING
#include "src/MQTTModule/MQTTNetworking.h"
#endif
#ifdef USE_BLE_SERVER_NETWORKING
#include "src/BLEServerModule/BLEServerNetworking.h"
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
#include "src/BLEClientModule/BLEClientNetworking.h"
#endif
#ifdef USE_BUTTON_MODULE
#include "src/ButtonModule/ButtonProcessing.h"
#endif
#ifdef USE_STEPPER_MODULE
#include "src/StepperModule/StepperModule.h"
#endif
#ifdef USE_UI_MODULE
#include "src/UIModule/UI.h"
#endif
#ifdef USE_WIFI_AP_MODULE
#include "src/WIFI_APModule/WIFI_APModule.h"
#endif
#ifdef USE_CAMERA_MODULE
#include "src/CameraModule/CameraModule.h"
#endif
#ifdef USE_AUDIO_MODULE
#include "src/AudioModule/AudioModule.h"
#endif

#ifdef ATOM_QRCODE_MODULE
#include "src/ATOM_QRCode_Module/ATOMQRCodeModule.h"
#endif

//! 12.26.23
#ifdef ATOM_SOCKET_MODULE
#include "src/ATOM_SocketModule/ATOM_SocketModule.h"
#endif

//! 1.6.24
#ifdef M5CORE2_MODULE
#include "src/M5Core2Module/M5Core2Module.h"
#endif


//! 1.22.24 refactored it out to just M5
#ifdef M5BUTTON_MODULE
#include "src/M5Module/M5ButtonModule.h"
#endif

//! try a call..
#ifdef USE_REST_MESSAGING
#include "src/MQTTModule/RESTMessaging.h"
#endif

//! The following are ALWAYS included as the "Main" functionality

//! add the MainModule.h so the defines for preferences are known
#include "src/MainModule/MainModule.h"

//! the feed
extern void feed();
#endif
