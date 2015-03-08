//
//  ScdfSensorAPI.cpp
//  ScdfController
//
//  Created by Marco Bertola on 25/01/15.
//
//

#include "ScdfSensorAPI.h"
#include "PipesManager.h"
#include "SensorsManager.h"
#include "SaveSettings.h"
#include "UDPSendersManager.h"
#include "UDPSender.h"
#include "Harvester.h"

const int max_number_ip_port = 65535;


using namespace scdf;


ScdfSensorAPI *scdf::theSensorAPI()
{
    static ScdfSensorAPI *instance=new ScdfSensorAPI();
    return instance;
}


// ********************************* Init API ****************************** //

#pragma mark Init

void ScdfSensorAPI::InitFramework()
{
   
    scdf::thePipesManager()->CreateReturnPipes();
    scdf::theSensorManager()->CreateAllSensors();
    
    InitNetwork();
    InitSensors();
    
    scdf::Harvester::Instance()->Start();
    
    StartAllSensors();
    

}

void ScdfSensorAPI::StartAllSensors()
{
    //scdf::theSensorManager()->StartSensor(scdf::SensorType::AudioInput);
    scdf::theSensorManager()->StartSensor(scdf::SensorType::Accelerometer);
    scdf::theSensorManager()->StartSensor(scdf::SensorType::Magnetometer);
    scdf::theSensorManager()->StartSensor(scdf::SensorType::Gyroscope);
    scdf::theSensorManager()->StartSensor(scdf::SensorType::Proximity);

}

void ScdfSensorAPI::InitSensors()
{
    scdf::SensorSettings settings;
    settings.rate = DEFAULT_SENSORS_RATE;
    
    scdf::theSensorManager()->InitSensor(scdf::Accelerometer, settings);
    scdf::theSensorManager()->InitSensor(scdf::Magnetometer, settings);
    scdf::theSensorManager()->InitSensor(scdf::Gyroscope, settings);
    scdf::theSensorManager()->InitSensor(scdf::Proximity, settings);
    
    scdf::SensorAudioSettings settingsAudio;
    settingsAudio.rate = DEFAULT_SAMPLE_RATE;
    settingsAudio.bufferSize = DEFAULT_BUFER_SIZE;
    settingsAudio.numChannels = DEFAULT_NUMBER_OF_CHANNEL;
    scdf::theSensorManager()->InitSensor(scdf::AudioInput, settingsAudio);
    
    
}

void ScdfSensorAPI::InitNetwork()
{
    // Load values from settings
    std::string addressString;
    int actualPort;
    std::string *ip = scdf::SaveSettings::Instance()->LoadStringValue(ADDRESS_IP_KEY);
    s_bool multiOutput = scdf::SaveSettings::Instance()->LoadBoolValue(MULTI_ROUTE_KEY);
    s_bool routingType = scdf::SaveSettings::Instance()->LoadBoolValue(OUTPUT_TYPE);
    actualPort = scdf::SaveSettings::Instance()->LoadIntValue(PORT_IP_KEY);
    
    // Check Values from settings
    if(ip)
    {
        addressString = *ip;
        delete ip;
        ip = NULL;
    } else
        addressString = DEFAULT_IP_ADDRESS;
    
    if (!actualPort) {
        actualPort = DEFAULT_UDP_PORT_BASE;
    }
    
    
    SetUDPOutputPort(actualPort);
    SetUDPOutputAddress(addressString);
    SetUDPMultiOutputActive(multiOutput);
    SetRoutingType(routingType);
}



// ********************************* Network API ****************************** //

#pragma mark Network API

void ScdfSensorAPI::ActivateSender(s_bool activate)
{
    scdf::UDPSendersManager::Instance()->GetSender()->Activate(activate);
}

void ScdfSensorAPI::SetRoutingType(s_bool useOscPackaging)
{
    scdf::UDPSendersManager::Instance()->SetUseOSCPackaging(true==useOscPackaging);
}

void ScdfSensorAPI::SetUDPOutputPort(s_int32 outputUdpPort)
{
    if (outputUdpPort>max_number_ip_port) {
        outputUdpPort = max_number_ip_port;
    }
    
    scdf::UDPSendersManager::Instance()->SetOutputPort(outputUdpPort);
}

void ScdfSensorAPI::SetUDPOutputAddress(std::string outputUdpIP)
{
    scdf::UDPSendersManager::Instance()->SetOutputAddress(outputUdpIP);
}

void ScdfSensorAPI::SetUDPMultiOutputActive(s_bool active)
{
    scdf::UDPSendersManager::Instance()->SetMultiOutput(active);
}



// ********************************* Sensors API ****************************** //

#pragma mark sensors API

s_int32 ScdfSensorAPI::GetSensorRate(scdf::SensorType sensorType)
{
    return scdf::theSensorManager()->GetRate(sensorType);
}

void ScdfSensorAPI::SetSensorRate(SensorType sensorType, s_int32 rate)
{
    scdf::theSensorManager()->SetRate(sensorType, rate);
}

void ScdfSensorAPI::SetAudioInputBufferSize(s_int32 bufferSize)
{
    scdf::theSensorManager()->SetBufferSize(scdf::AudioInput, bufferSize);
}

s_bool ScdfSensorAPI::StartSensor(SensorType sensorType)
{
     return scdf::theSensorManager()->StartSensor(sensorType);
}

s_bool ScdfSensorAPI::StopSensor(SensorType sensorType)
{
    return scdf::theSensorManager()->StopSensor(sensorType);
}

s_bool ScdfSensorAPI::IsSensorActive(SensorType sensorType)
{
    return scdf::theSensorManager()->IsSensorActive(sensorType);
}

// ********************************* Listeners ****************************** //

#pragma mark Listeners

void ScdfSensorAPI::SetHarvesterListener(HarvesterListener *listener)
{
    Harvester::Instance()->SetHarvesterListener(listener);
}
