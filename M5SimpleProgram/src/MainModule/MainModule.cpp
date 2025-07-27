
#include "MainModule.h"



#include "../../Defines.h"



//****** GLOBALS (shows how much memory is allocated.. )
//!the parameter being sent to those commands passing an argument
char _asyncParameter[500];
//!global to store credentials when ever they are stored..
char _JSONStringForWIFICredentials[200];
//!global for others to use..
#define MESSAGE_STORAGE_MAX 500
char _messageStorage[MESSAGE_STORAGE_MAX];
//!status string (URL query format)
char _fullStatusString[500];
//! current smMode
char _smMode_MainModule[10];
//! saved deviceName storage..
char _deviceNameSave[50];

//!store a big message  #MSG_START .. #MSG_END
boolean _waitingForBigMessageEnd = false;
char _bigMessage[500];
//!saved BLE connected name 8.26.22
char _connectedBLEDeviceName[50];
//!full: ""Name: PTFeeder:HowieFeeder, Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead"
char _fullBLEDeviceName[100];
//!full: ""Address: 7c:9e:bd:48:af:92, serviceUUID: 0xdead"
char _connectedBLEDeviceAddress[50];


//!gets the device name
char *deviceName_mainModule()
{
    return (char*)"M5Simple";
}


//!power of devices
//!reboot
void rebootDevice_mainModule()
{
    ESP.restart();
}

//! some preference
boolean getPreferenceBoolean_mainModule(int preferenceID)
{
    return true;
}

#pragma mark CALLBACKS

//! New RegisterCallback that works across a number of callback modules
//!
//! callbacksModuleId
#define CALLBACKS_MQTT          0
#define CALLBACKS_BUTTON_MODULE 1
#define CALLBACKS_BLE_CLIENT    2
#define CALLBACKS_BLE_SERVER    3



//!register the callback based on the callbackType. use the callbacksModuleId for which one..
void registerCallbackMain(int callbacksModuleId, int callbackType, void (*callback)(char*));
//!performs the indirect callback based on the callbackType
void callCallbackMain(int callbacksModuleId, int callbackType, char *message);


//!make sure these are the number of callbacks.. 0..n
//! These values are from the respective .h of the modules
#define CALLBACKS_MAX_MQTT          MQTT_MAX_CALLBACKS
#define CALLBACKS_MAX_BUTTON_MODULE MAX_CALLBACKS_BM
#define CALLBACKS_MAX_BLE_CLIENT    BLE_CLIENT_MAX_CALLBACKS
#define CALLBACKS_MAX_BLE_SERVER    BLE_SERVER_MAX_CALLBACKS


//!example callback
void dummyCallbackMain(char *message)
{
    SerialLots.printf("No callback defined .. %s\n", message);
}

//tricky getting array of arrays (or pointers to arrays), without dynamic memory.. we are going to use calloc()
//https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
//https://www.tutorialspoint.com/cprogramming/c_pointer_to_pointer.htm
//https://stackoverflow.com/questions/5573302/pointer-to-an-array-of-function-pointers
//!the array of callback functions
//* this means there are arrays of N pointers to functions that take 1 parameter (char*).


//! the max of each module .. hard coded (or 0 if module not there)
int _callbacksFunctionsMAXS[CALLBACKS_MODULE_MAX];

//!flag for initializing if not yes
boolean _callbacksInitializedMain = false;

//a pointer to a callback function that takes (char*) and returns void
typedef void (*callbackSignature)(char *);
//callbackSignature fpar[2] = {&dummyCallbackMain, &dummyCallbackMain};

//!return dyamically created array of max
callbackSignature * createMemory(int max)
{
    SerialLots.printf("creatingCallbacks %d size: %d\n", max, sizeof(callbackSignature *));
    callbackSignature *callbackArray = (callbackSignature *)calloc(max,sizeof(callbackSignature *));
    for (int i=0; i< max; i++)
    {
        callbackArray[i] = &dummyCallbackMain;
    }
    return callbackArray;
}
//!array of known size (CALLBACKS_MODULE_MAX) of callbackSignatures
callbackSignature *_allCallbacks[CALLBACKS_MODULE_MAX];

//!init the callbacks to dummy callbacks
void initCallbacksMain()
{
    if (!_callbacksInitializedMain)
    {
        _callbacksInitializedMain = true;
        
        // default to 0 callbacks.. in case module not there..
        for (int i = 0; i < CALLBACKS_MODULE_MAX; i++)
        {
            _callbacksFunctionsMAXS[i] = 0;
        }
        //! only place for #ifdef  (NOTE some can be 0 based on the #ifdef module not being included..
#ifdef USE_MQTT_NETWORKING
        _callbacksFunctionsMAXS[CALLBACKS_MQTT] = CALLBACKS_MAX_MQTT;
#endif
#ifdef USE_BLE_SERVER_NETWORKING
        _callbacksFunctionsMAXS[CALLBACKS_BLE_SERVER] = CALLBACKS_MAX_BLE_SERVER;
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
        _callbacksFunctionsMAXS[CALLBACKS_BLE_CLIENT] = CALLBACKS_MAX_BLE_CLIENT;
#endif
#ifdef USE_BUTTON_MODULE
        _callbacksFunctionsMAXS[CALLBACKS_BUTTON_MODULE] = CALLBACKS_MAX_BUTTON_MODULE;
#endif
        // initialize the dummy callbacks
        for (int i = 0; i < CALLBACKS_MODULE_MAX; i++)
        {
            //first determine how many are in the array..
            int max = _callbacksFunctionsMAXS[i];
            //then create the space which are (void *) pointers
            _allCallbacks[i] = createMemory(max);
        }
    }
}
//!register the callback based on the callbackType. use the callbacksModuleId for which one..
void registerCallbackMain(int callbacksModuleId, int callbackType, void (*callback)(char*))
{
    //init if not already..
    initCallbacksMain();
    int max = _callbacksFunctionsMAXS[callbacksModuleId];
    
    if (callbackType < 0 || callbackType >= max)
    {
        SerialError.printf("#### Error outside callback range - 1, %d\n", callbackType);
    }
    else
    {
        // array of arrays (or array of pointer to another array)
        _allCallbacks[callbacksModuleId][callbackType] = callback;
    }
}
//!performs the indirect callback based on the callbackType
void callCallbackMain(int callbacksModuleId, int callbackType, char *message)
{
    //init if not already..
    initCallbacksMain();
    int max = _callbacksFunctionsMAXS[callbacksModuleId];
    
    if (callbackType < 0 || callbackType >= max)
    {
        SerialError.printf("#### Error outside callback range - 2, %d\n", callbackType);
        
    }
    else {
        void (*callbackFunction)(char *) = _allCallbacks[callbacksModuleId][callbackType];
        (*callbackFunction)(message);
    }
}

#pragma mark CALLBACKS

/*******************************BLE Server*************************************/
//!The callback for "onWrite" of the bluetooth "onWrite'
void onBLEServerCallback(char *message)
{
    SerialCall.print("onWriteBLEServerCallback: ");
    SerialCall.println(message);
    
}
//!  The callback for "status messages" of the bluetooth
void onStatusMessageBLEServerCallback(char *message)
{
    SerialCall.print("onStatusMessageBLEServerCallback");
    SerialCall.print(message);
    //! 2.2.22  send this over bluetooth..  TODO.
    //
}

//! The FINISH of  callback for "onWrite" of the bluetooth "onWrite'
void onWriteBLEServerCallbackFinish(char *message)
{
    
    SerialDebug.print("onWriteBLEServerCallback_main: ");
    SerialDebug.println(message);
    
    char cmd = message[0];

    switch (cmd)
    {
        case 'c':
        case 's':
        case 0x00:
            feed();
            break;
            
        case 'r':
            rebootDevice_mainModule();
            break;
    }
    
    
#ifdef LATER
    //!client never sends these wrapped messages of length 1, so process normally..
    //!gets us out of potential infinite wait
    if (strlen(message) == 1)
        _waitingForBigMessageEnd = false;
    
    if (strcmp(message, "#MSG_START")==0)
    {
        _waitingForBigMessageEnd = true;
        strcpy(_bigMessage,"");
        return;
    }
    else if (_waitingForBigMessageEnd)
    {
        if (strcmp(message, "#MSG_END")==0)
        {
            _waitingForBigMessageEnd = false;
            //! we are done.. send big message to ourself..
            onWriteBLEServerCallback(_bigMessage);
        }
        else
        {
            strcat(_bigMessage,message);
            return;
        }
    }
    _waitingForBigMessageEnd = false;
    
    //!Issue: the message is arriving via BLE, but if we call the MQTT process, it might call us back to send
    //!the message over BLE. This hangs things up!
    //!So short optimization for 'feed' will be to look for cmd and feed in message {cmd:feed}
    //!8.19.22  issue #162
    if (containsSubstring(message,"cmd"))
    {
        if (containsSubstring(message,"feed"))
        {
            message = (char*)"s";
        }
        else if (containsSubstring(message,"buzzOn"))
        {
            message = (char*)"B";
        }
        else if (containsSubstring(message,"buzzOff"))
        {
            message = (char*)"b";
        }
        
    }
    
#ifdef USE_MQTT_NETWORKING
    //This should be a 'register' command..
    
    //!the MQTTNetwork.processJSONMessage()
    //!will return true if the message was JSON and was processes, false otherwise.
    if (processJSONMessageMQTT(message, NULL))
    {
        //!processed by the MQTTNetworking code..
#ifdef USE_BLE_SERVER_NETWORKING
        sendBLEMessageACKMessage();
#endif
        //pCharacteristic->setValue(0x01);  //??  This is the acknowlege(ACK) back to client.  Later this should be contigent on a feed completed
    }
    else
    {
        //!perform feed...
        SerialDebug.printf("Perform BLE Command '%s'\n", message);
        
        char cmd = message[0];
        
        //OOOPS.. we need the ProcessClientCmd .. for other than just step..
        // for sensors..
        
        //!look for ** sensor commands..
        processClientCommandChar_mainModule(cmd);
        
#ifdef USE_BLE_SERVER_NETWORKING
        sendBLEMessageACKMessage();
#endif
        
    }
#endif
    SerialDebug.println("*********");
    
#endif
} //onWriteBLEServerCallback

//! The callback for "onWrite" of the bluetooth "onWrite'
void onWriteBLEServerCallback(char *message)
{
    //TEST:
#define TRY_ASYNC_BLE_PROCESSING_
#ifdef TRY_ASYNC_BLE_PROCESSING
    
    //!send an async call with a string parameter. This will set store the value and then async call the command (passing the parameter)
    //!These are the ASYNC_CALL_PARAMETERS_MAX
    main_dispatchAsyncCommandWithString(ASYNC_JSON_MESSAGE_PARAMETER, message);
#else
    
    //! The callback for "onWrite" of the bluetooth "onWrite'
    onWriteBLEServerCallbackFinish(message);
#endif
}

#pragma mark MAIN

//!init globals strings
void initGlobals_mainModule()
{
    //!NOTE: this is definitely needed, as grabbing strings willy nilly can bomb or corrupt stuff..
    strcpy(_connectedBLEDeviceName,(char*)"");
    strcpy(_bigMessage,(char*)"");
    strcpy(_deviceNameSave,(char*)"MyDeviceName");
    strcpy(_smMode_MainModule,(char*)"");
    strcpy(_fullStatusString,(char*)"");
    strcpy(_messageStorage,(char*)"");
    strcpy(_JSONStringForWIFICredentials,(char*)"");
    strcpy(_asyncParameter,(char*)"");
    strcpy(_fullBLEDeviceName,(char*)"");
    
//    main_setScannedDeviceName((char*)"");
//    //! 1.7.24
//    main_setScannedGroupName((char*)"");
    
}

//!called for things like the advertisement
char *getDeviceNameMQTT()
{
    
    return _deviceNameSave;
    // return _chipName;
}

//!THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//!This will perform preference initializtion as well
//! called from the setup()
void setup_mainModule()
{
    SerialMin.println("setup_mainModule");

    initCallbacksMain();
    
    
    //!init globals like strings (but nothing that needs preferences)
    initGlobals_mainModule();
    
    //*** The callback for "onWrite" of the bluetooth "onWrite'
    registerCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_ONWRITE, &onWriteBLEServerCallback);
    //*** The callback for "onWrite" of the bluetooth "onWrite'
    registerCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_STATUS_MESSAGE, &onStatusMessageBLEServerCallback);
    registerCallbackMain(CALLBACKS_BLE_SERVER, BLE_SERVER_CALLBACK_STATUS_MESSAGE, &onStatusMessageBLEServerCallback);
    
    //strdup() get away from the
    setup_BLEServerNetworking(MAIN_BLE_SERVER_SERVICE_NAME, getDeviceNameMQTT(), strdup(PT_SERVICE_UUID), strdup(PT_CHARACTERISTIC_UUID));
    
    SerialDebug.println("done setupBLEServerNetworking");
    
}

//! called for the loop() of this plugin
void loop_mainModule()
{
    
}

