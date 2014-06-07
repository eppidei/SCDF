//
//  ScdfPipe.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "Sensor.h"
#include "ScdfPipe.h"
#include "unistd.h"
#include <vector>

namespace scdf
{
    std::vector<ScdfPipe> pipes;
    void CreatePipes()
    {
        pipes.resize(SensorType::NumTypes);
    }
}

using namespace scdf;

void ScdfPipe::Close()
{
    if(pd[0]!=-1)
        close(pd[0]);
    if(pd[1]!=-1)
        close(pd[1]);
}

void ScdfPipe::Open()
{
    if(pipe(pd) == -1)
    {
        //Logging::Instance()->log(_T("Error creating pipe"));
        invalid=true;
    }
}

s_bool ScdfPipe::Read(s_char *buffer, s_int32 bytesToRead, s_int32 *bytesRead)
{
    s_int32 bytesRead1=0;
    bytesRead1=read(pd[0], buffer, bytesToRead);
    if(bytesRead)
        *bytesRead=bytesRead1;
    return -1!=bytesRead1;
}
s_bool ScdfPipe::Write(const s_char *buffer, s_int32 bytesToWrite, s_int32 *bytesWritten)
{
    s_int32 bytesWritten1=write(pd[1], buffer, bytesToWrite);
    if(bytesWritten) *bytesWritten=bytesWritten1;
    return bytesWritten1!=-1;
}

template s_int32 ScdfPipe::SendMessage<SensorData*> (SensorData *msg);

template <class PipeMessage> s_int32 ScdfPipe::SendMessage(PipeMessage msg)
{
    //_ASSERT(!invalid);
    if (invalid) return 0;
    s_int32 bytesWritten=0;
    s_int32 ret=Write((const s_char *)msg, sizeof(PipeMessage), &bytesWritten);
    if(!ret)
    {
        //Logging::Instance()->log(_T("Error writing to pipe"));
        return 0;
    }
    //_ASSERT(bytesWritten==sizeof(PipeMessage));
    return 1;
}

template SensorData* ScdfPipe::ReadMessage<SensorData*>();

template <class PipeMessage> PipeMessage ScdfPipe::ReadMessage()
{
    //_ASSERT(!invalid);
    if (invalid) return NULL;
    s_int32 numBytesRead=0;
    PipeMessage msg;
    if (Read((s_char *)msg, sizeof(PipeMessage), &numBytesRead))
    {
        //Logging::Instance()->log(_T("Error reading from pipe"));
        return NULL;
    }
    
    if(0==numBytesRead) return NULL;
    //_ASSERT(numBytesRead==sizeof(PipeMessage));
    return msg;
}

ScdfPipe::ScdfPipe() : invalid(false)
{
    Open();
}

ScdfPipe::~ScdfPipe()
{
    if (invalid) return;
    Close();
}
