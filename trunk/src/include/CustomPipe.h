//
//  ScdfPipe.h
//  scdf
//
//  Created by Andrea Scuderi on 01/06/14.

#include "TypeDefinitions.h"

namespace scdf
{
    typedef s_int32 PipeDescriptor;

    class CustomPipe
    {
        PipeDescriptor pd[2];
        s_bool invalid;
        s_int32 size;
    
        void Close();
        void Open();
        s_bool Read(s_char *buffer, s_int32 bytesToRead, s_int32 *bytesRead);
        s_bool Write(const s_char *buffer, s_int32 bytesToWrite, s_int32 *bytesWritten);
        void SetBlockingCalls(PipeDescriptor pd);
        void SetNonBlockingCalls(PipeDescriptor pd);
        
    public:
        s_int32 GetSize() { return size; }
        void SetBlockingReads();
        void SetNonBlockingReads();
        void SetBlockingWrites();
        void SetNonBlockingWrites();
        template <class PipeMessage> s_int32 WriteMessage(PipeMessage msg);
        template <class PipeMessage> PipeMessage ReadMessage();
        CustomPipe();
        ~CustomPipe();
    };

    std::vector<CustomPipe*> *GetReturnPipes();

}




