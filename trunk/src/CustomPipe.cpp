//
//  ScdfPipe.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//
#include "Sensor.h"
#include "Logging.h"
#include "CustomPipe.h"

using namespace scdf;

std::vector<CustomPipe*> pipes;
std::vector<CustomPipe*> returnPipes;

std::vector<CustomPipe*>* scdf::GetReturnPipes() { return &returnPipes; }

void CreatePipes()
{
    for (int i=0;i<NumTypes;++i)
        pipes.push_back(new CustomPipe());
}

void CreateReturnPipes()
{
    for (int i=0;i<NumTypes;++i)
    {
        CustomPipe *p=new CustomPipe();
        p->SetNonBlockingReads();
        p->SetNonBlockingWrites();
        returnPipes.push_back(p);
    }
}

void InitReturnPipes(SensorType type, s_int32 numSamples)
{
    while(true)
    {
        SensorData *s=returnPipes[type]->ReadMessage<SensorData*>();
        if (NULL==s) break;
        delete s;
    }
    for (int j=0;j<RETURN_PIPES_STATIC_INIT;++j)
    {
        if (type==AudioInput)
        {
            SensorAudioData *s=new SensorAudioData();
            s->data=new s_sample[numSamples];
            if (0==returnPipes[type]->WriteMessage<SensorData*>(s))
                delete s;
        }
        else
        {
            SensorData *s=new SensorData();
            s->data=new s_sample[numSamples];
            if (0==returnPipes[type]->WriteMessage<SensorData*>(s))
                delete s;
        }
    }
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
        LOGD("Error creating pipe");
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
    assert(!invalid);
    if (invalid) return 0;
    s_int32 bytesWritten=0;
    s_int32 ret=Write((const s_char *)&msg, sizeof(PipeMessage), &bytesWritten);
    if(!ret)
    {
        LOGD("Error writing to pipe");
        return 0;
    }
    assert(bytesWritten==sizeof(PipeMessage));
    size++;
    return 1;
}

template SensorData* CustomPipe::ReadMessage<SensorData*>();

template <class PipeMessage> PipeMessage CustomPipe::ReadMessage()
{
    assert(!invalid);
    if (invalid) return NULL;
    s_int32 numBytesRead=0;
    PipeMessage p;
    if (!Read((s_char*)&p, sizeof(PipeMessage), &numBytesRead))
    {
        //LOGD("Error reading from pipe");
        return NULL;
    }
    if(0==numBytesRead) return NULL;
    assert(numBytesRead==sizeof(PipeMessage));
    size--;
    return p;
}

CustomPipe::CustomPipe() : invalid(false), size(0)
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