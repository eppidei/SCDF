//
//  ScdfPipe.h
//  scdf
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "TypeDefinitions.h"

namespace scdf
{
    class SensorData;
    typedef s_int32 PipeDescriptor;
    typedef SensorData* PipeMessage;

    class ScdfPipe
    {
        PipeDescriptor pd[2];
        const s_int32 SizeofPipeMessage=sizeof(PipeMessage);
        s_bool invalid;
    
        void Close();
        void Open();
        s_bool Read(s_char *buffer, s_int32 bytesToRead, s_int32 *bytesRead);
        s_bool Write(const s_char *buffer, s_int32 bytesToWrite, s_int32 *bytesWritten);
    public:
        s_int32 SendMessage(PipeMessage msg);
        PipeMessage ReadMessage();
        ScdfPipe();
        ~ScdfPipe();
    };
}




