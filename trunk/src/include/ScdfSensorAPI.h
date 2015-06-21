//
//  ScdfSensorAPI.h
//  ScdfController
//
//  Created by Marco Bertola on 25/01/15.
//
//

#ifndef __ScdfController__ScdfSensorAPI__
#define __ScdfController__ScdfSensorAPI__

#include "TypeDefinitions.h"
#include "Sensor.h"

const int DEFAULT_SAMPLE_RATE = 44100;
const int DEFAULT_BUFER_SIZE = 512;
const int DEFAULT_NUMBER_OF_CHANNEL = 1;
const int DEFAULT_SENSORS_RATE = 50;



namespace scdf {
    
    class HarvesterListener
    {
       public:
            virtual void OnHarvesterBufferReady(std::vector<SensorData*> *buffer) = 0;
            virtual void Init(s_int32 numFrames, s_int32 rate) = 0;
            virtual void Release() = 0;
    };
    
    class SCDF_API ScdfSensorAPI
    {
        ScdfSensorAPI(){}
        
        void InitSensors();
        void InitNetwork();
        
    public:
        
        friend ScdfSensorAPI *theSensorAPI();
        
        void InitFramework();
        void StartAllSensors(bool excludeAudio = false);
        void StopAllSensors();
        
        // *********** SENSORS API ******************* //
        s_int32 GetSensorRate(scdf::SensorType sensorType);
        void SetSensorRate(SensorType sensorType, s_int32 rate);
        void SetAudioInputBufferSize(s_int32 bufferSize);
        s_bool StartSensor(SensorType sensorType);
        s_bool StopSensor(SensorType sensorType);
        s_bool IsSensorActive(SensorType sensorType);
        s_bool SensorExists(SensorType sensorType);
        
        // *********** NETWORK API ******************* //
        void SetUDPOutputPort(s_int32 outputUdpPort);
        void SetUDPOutputAddress(std::string outputUdpIP);
        void SetUDPMultiOutputActive(s_bool active);
        void SetRoutingType(s_bool useOscPackaging);
        void ActivateSender(s_bool activate);
        
         // *********** LISTNERS ******************* //
        void AttachHarvesterListener(HarvesterListener* _listener,std::vector<SensorType> _typeList );
        void DetachHarvesterListener(HarvesterListener* _listener);
    };

    
    ScdfSensorAPI * SCDF_API theSensorAPI();
}




#endif /* defined(__ScdfController__ScdfSensorAPI__) */
