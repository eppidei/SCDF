//
//  ScdfPipe.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//
#include <string>
#include "Sensor.h"
#include "CustomPipe.h"
#include "unistd.h"
#include <vector>

using namespace scdf;

std::vector<CustomPipe*> pipes;
void CreatePipes()
{
    for (int i=0;i<SensorType::NumTypes;++i)
        pipes.push_back(new CustomPipe());
}

void CustomPipe::Close()
{
    if(pd[0]!=-1)
        close(pd[0]);
    if(pd[1]!=-1)
        close(pd[1]);
}

void CustomPipe::Open()
{
    if(pipe(pd) == -1)
    {
        //Logging::Instance()->log(_T("Error creating pipe"));
        invalid=true;
    }
}

s_bool CustomPipe::Read(s_char *buffer, s_int32 bytesToRead, s_int32 *bytesRead)
{
    s_int32 bytesRead1=0;
    bytesRead1=read(pd[0], buffer, bytesToRead);
    if(bytesRead)
        *bytesRead=bytesRead1;
    return -1!=bytesRead1;
}
s_bool CustomPipe::Write(const s_char *buffer, s_int32 bytesToWrite, s_int32 *bytesWritten)
{
    s_int32 bytesWritten1=write(pd[1], buffer, bytesToWrite);
    if(bytesWritten) *bytesWritten=bytesWritten1;
    return bytesWritten1!=-1;
}

template s_int32 CustomPipe::WriteMessage<SensorData*> (SensorData *msg);

template <class PipeMessage> s_int32 CustomPipe::WriteMessage(PipeMessage msg)
{
    //_ASSERT(!invalid);
    if (invalid) return 0;
    s_int32 bytesWritten=0;
    s_int32 ret=Write((const s_char *)&msg, sizeof(PipeMessage), &bytesWritten);
    if(!ret)
    {
        //Logging::Instance()->log(_T("Error writing to pipe"));
        return 0;
    }
    //_ASSERT(bytesWritten==sizeof(PipeMessage));
    return 1;
}

template SensorData* CustomPipe::ReadMessage<SensorData*>();

template <class PipeMessage> PipeMessage CustomPipe::ReadMessage()
{
    //_ASSERT(!invalid);
    if (invalid) return NULL;
    s_int32 numBytesRead=0;
    PipeMessage p;
    if (!Read((s_char*)&p, sizeof(PipeMessage), &numBytesRead))
    {
        //Logging::Instance()->log(_T("Error reading from pipe"));
        return NULL;
    }
    if(0==numBytesRead) return NULL;
    //_ASSERT(numBytesRead==sizeof(PipeMessage));
    return p;
}

CustomPipe::CustomPipe() : invalid(false)
{
    Open();
}

CustomPipe::~CustomPipe()
{
    if (invalid) return;
    Close();
}

void CustomPipe::SetBlockingCalls(PipeDescriptor pd)
{
    s_int32 opts = fcntl(pd,F_GETFL);
    opts = opts & (~O_NONBLOCK);
    fcntl(pd,F_SETFL,opts);
}

void CustomPipe::SetNonBlockingCalls(PipeDescriptor pd)
{
    s_int32 opts = fcntl(pd,F_GETFL);
    opts = (opts | O_NONBLOCK);
    fcntl(pd,F_SETFL,opts);
}

void CustomPipe::SetBlockingReads()
{
    SetBlockingCalls(pd[0]);
}

void CustomPipe::SetNonBlockingReads()
{
    SetNonBlockingCalls(pd[0]);
}
void CustomPipe::SetBlockingWrites()
{
    SetBlockingCalls(pd[1]);
}

void CustomPipe::SetNonBlockingWrites()
{
    SetNonBlockingCalls(pd[1]);
}
