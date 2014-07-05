//
//  PipesManager.h
//  SCDF
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "Sensor.h"

namespace scdf
{
    class CustomPipe;
    class PipesManager;
    PipesManager *thePipesManager();
    class PipesManager
    {
        std::vector<CustomPipe*> pipes;
        std::vector<CustomPipe*> returnPipes;
        PipesManager() {LOGD("Pipes manager constructor");}
        void CreatePipes();
    public:
        void CreateReturnPipes();
        std::vector<CustomPipe*>* GetReturnPipes();
        void InitReturnPipes(SensorType type, s_int32 numSamples);
        friend PipesManager *thePipesManager();
        SensorData *ReadFromPipe(SensorType type);
        SensorData *ReadFromReturnPipe(SensorType type);
        s_int32 WriteOnReturnPipe(SensorType type, SensorData *data);
        s_int32 WriteOnPipe(SensorType type, SensorData *data);
        s_int32 NumPipes();
        void InitPipes();
    };

}
