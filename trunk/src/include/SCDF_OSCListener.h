#include <stdio.h>
#include <iostream>
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
//#include "my_defines.h"
//#include "my_typedefs.h"




class SCDFPacketListener : public osc::OscPacketListener {


public :

	float *accel_data_buff_p;
	float *gyro_data_buff_p;
	float *magneto_data_buff_p;
	float *proxy_data_buff_p;
	float *light_data_buff_p;
	float *audio_data_buff_p;
	s_uint64 *accel_timestamps_buff_p;
	s_uint64 *gyro_timestamps_buff_p;
	s_uint64 *magneto_timestamps_buff_p;
	s_uint64 *proxy_timestamps_buff_p;
	s_uint64 *light_timestamps_buff_p;
	s_uint64 *audio_timestamps_buff_p;
	int n_accel_data;
	int n_gyro_data;
	int n_magneto_data;
	int n_proxy_data;
	int n_light_data;
	int n_audio_data;
	s_int64 accel_timeid;
		s_int64 gyro_timeid  ;
		s_int64 magneto_timeid ;
		s_int64 proxy_timeid;
		s_int64 light_timeid;
		s_int64 audio_timeid;


	SCDFPacketListener(int size, int size2)
	{
		accel_data_buff_p = new float[size];
		accel_timestamps_buff_p = new s_uint64[size];

		gyro_data_buff_p = new float[size];
		gyro_timestamps_buff_p = new s_uint64[size];

		magneto_data_buff_p = new float[size];
		magneto_timestamps_buff_p = new s_uint64[size];

		proxy_data_buff_p = new float[size];
		proxy_timestamps_buff_p = new s_uint64[size];

		light_data_buff_p = new float[size];
		light_timestamps_buff_p = new s_uint64[size];

		audio_data_buff_p = new float[size2];
		audio_timestamps_buff_p = new s_uint64[size2];
	
		n_accel_data = 0;
		n_gyro_data  = 0;
		n_magneto_data = 0;
		n_proxy_data= 0;
		n_light_data= 0;
		n_audio_data= 0;
		
		accel_timeid = 0;
		gyro_timeid  = 0;
		magneto_timeid = 0;
		proxy_timeid= 0;
		light_timeid= 0;
		audio_timeid= 0;
	
	}
	~SCDFPacketListener()
	{
		delete []accel_data_buff_p;
		delete []gyro_data_buff_p;
		delete []magneto_data_buff_p;
		delete []proxy_data_buff_p;
		delete []light_data_buff_p;
		delete []audio_data_buff_p;
		delete []accel_timestamps_buff_p;
		delete []gyro_timestamps_buff_p;
		delete []magneto_timestamps_buff_p;
		delete []proxy_timestamps_buff_p;
		delete []light_timestamps_buff_p;
		delete []audio_timestamps_buff_p;
	}
protected:


    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )
    {
        (void) remoteEndpoint; // suppress unused parameter warning

		
        try{
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.
			char *pch=NULL;
			

			pch = strtok ( (char*)m.AddressPattern(),"/");
		
			if (std::strcmp(pch,"Accelerometer")==0)
			{
				pch = strtok ( NULL,"/");

				if (std::strcmp(pch,"Frames")==0)
				{
					Get_sensor_Frame(m,remoteEndpoint,&num_frames);
				}
				else if(std::strcmp(pch,"Rate")==0)
				{
					Get_sensor_Rate(m,remoteEndpoint);
				}
				else if(std::strcmp(pch,"Channels")==0)
				{
					Get_sensor_Channels(m,remoteEndpoint,&num_chs);
				}
				else if(std::strcmp(pch,"TimeID")==0)
				{
					Get_sensor_TimeID(m,remoteEndpoint,&accel_timeid);
				}
				else if(std::strcmp(pch,"Data")==0)
				{
					num_samples = num_frames*num_chs;
					n_accel_data = num_samples;
					byte_size=num_samples*4;
					Get_sensor_Data(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,accel_data_buff_p);
				}
				else if(std::strcmp(pch,"Timestamps")==0)
				{
					byte_size=2*sizeof(s_uint64);
					Get_sensor_Timestamps(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,accel_timestamps_buff_p);
				}
				else
				{
#ifdef VERBOSE_OSC
					printf("unknown Accelerometer address\n");
#endif
				}

			}
			else if (std::strcmp(pch,"Gyroscope")==0)
			{
				pch = strtok ( NULL,"/");

				if (std::strcmp(pch,"Frames")==0)
				{
					Get_sensor_Frame(m,remoteEndpoint,&num_frames);
				}
				else if(std::strcmp(pch,"Rate")==0)
				{
					Get_sensor_Rate(m,remoteEndpoint);
				}
				else if(std::strcmp(pch,"Channels")==0)
				{
					Get_sensor_Channels(m,remoteEndpoint,&num_chs);
				}
				else if(std::strcmp(pch,"TimeID")==0)
				{
					Get_sensor_TimeID(m,remoteEndpoint,&gyro_timeid);
				}
				else if(std::strcmp(pch,"Data")==0)
				{
					num_samples = num_frames*num_chs;
					n_gyro_data = num_samples;
					byte_size=num_samples*4;
					Get_sensor_Data(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,gyro_data_buff_p);
				}
				else if(std::strcmp(pch,"Timestamps")==0)
				{
					byte_size=2*sizeof(s_uint64);
					Get_sensor_Timestamps(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,gyro_timestamps_buff_p);
				}
				else
				{
					#ifdef VERBOSE_OSC
						printf("unknown Gyroscope address\n");
#endif
				}
			}
			else if (std::strcmp(pch,"Magnetometer")==0)
			{
				pch = strtok ( NULL,"/");

				if (std::strcmp(pch,"Frames")==0)
				{
					Get_sensor_Frame(m,remoteEndpoint,&num_frames);
				}
				else if(std::strcmp(pch,"Rate")==0)
				{
					Get_sensor_Rate(m,remoteEndpoint);
				}
				else if(std::strcmp(pch,"Channels")==0)
				{
					Get_sensor_Channels(m,remoteEndpoint,&num_chs);
				}
				else if(std::strcmp(pch,"TimeID")==0)
				{
					Get_sensor_TimeID(m,remoteEndpoint,&magneto_timeid);
				}
				else if(std::strcmp(pch,"Data")==0)
				{
					num_samples = num_frames*num_chs;
					n_magneto_data = num_samples;
					byte_size=num_samples*4;
					Get_sensor_Data(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,magneto_data_buff_p);
				}
				else if(std::strcmp(pch,"Timestamps")==0)
				{
					byte_size=2*sizeof(s_uint64);
					Get_sensor_Timestamps(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,magneto_timestamps_buff_p);
				}
				else
				{
					#ifdef VERBOSE_OSC
					printf("unknown Magnetometer address\n");
#endif
				}
			}
			else if (std::strcmp(pch,"Proximity")==0)
			{
				pch = strtok ( NULL,"/");

				if (std::strcmp(pch,"Frames")==0)
				{
					Get_sensor_Frame(m,remoteEndpoint,&num_frames);
				}
				else if(std::strcmp(pch,"Rate")==0)
				{
					Get_sensor_Rate(m,remoteEndpoint);
				}
				else if(std::strcmp(pch,"Channels")==0)
				{
					Get_sensor_Channels(m,remoteEndpoint,&num_chs);
				}
				else if(std::strcmp(pch,"TimeID")==0)
				{
					Get_sensor_TimeID(m,remoteEndpoint,&proxy_timeid);
				}
				else if(std::strcmp(pch,"Data")==0)
				{
					num_samples = num_frames*num_chs;
					n_proxy_data = num_samples;
					byte_size=num_samples*4;
					Get_sensor_Data(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,proxy_data_buff_p);
				}
				else if(std::strcmp(pch,"Timestamps")==0)
				{
					byte_size=2*sizeof(s_uint64);
					Get_sensor_Timestamps(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,proxy_timestamps_buff_p);
				}
				else
				{
					#ifdef VERBOSE_OSC
					printf("unknown Proximity address\n");
#endif
				}
			}
			else if (std::strcmp(pch,"Light")==0)
			{
				pch = strtok ( NULL,"/");

				if (std::strcmp(pch,"Frames")==0)
				{
					Get_sensor_Frame(m,remoteEndpoint,&num_frames);
				}
				else if(std::strcmp(pch,"Rate")==0)
				{
					Get_sensor_Rate(m,remoteEndpoint);
				}
				else if(std::strcmp(pch,"Channels")==0)
				{
					Get_sensor_Channels(m,remoteEndpoint,&num_chs);
				}
				else if(std::strcmp(pch,"TimeID")==0)
				{
					Get_sensor_TimeID(m,remoteEndpoint,&light_timeid);
				}
				else if(std::strcmp(pch,"Data")==0)
				{
					num_samples = num_frames*num_chs;
					n_light_data = num_samples;
					byte_size=num_samples*4;
					Get_sensor_Data(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,light_data_buff_p);
				}
				else if(std::strcmp(pch,"Timestamps")==0)
				{
					byte_size=2*sizeof(s_uint64);
					Get_sensor_Timestamps(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,light_timestamps_buff_p);
				}
				else
				{
					#ifdef VERBOSE_OSC
					printf("unknown Light address\n");
#endif
				}
			}
			else if (std::strcmp(pch,"AudioInput")==0)
			{
				pch = strtok ( NULL,"/");

				if (std::strcmp(pch,"Frames")==0)
				{
					Get_sensor_Frame(m,remoteEndpoint,&num_frames);
				}
				else if(std::strcmp(pch,"Rate")==0)
				{
					Get_sensor_Rate(m,remoteEndpoint);
				}
				else if(std::strcmp(pch,"Channels")==0)
				{
					Get_sensor_Channels(m,remoteEndpoint,&num_chs);
				}
				else if(std::strcmp(pch,"TimeID")==0)
				{
					Get_sensor_TimeID(m,remoteEndpoint,&audio_timeid);
				}
				else if(std::strcmp(pch,"Data")==0)
				{
					num_samples = num_frames*num_chs;
					n_audio_data = num_samples;
					byte_size=num_samples*4;
					Get_sensor_Data(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,audio_data_buff_p);
				}
				else if(std::strcmp(pch,"Timestamps")==0)
				{
					byte_size=2*sizeof(s_uint64);
					Get_sensor_Timestamps(m,remoteEndpoint,(osc::osc_bundle_element_size_t*)&byte_size,audio_timestamps_buff_p);
				}
				else
				{
					#ifdef VERBOSE_OSC
					printf("unknown AudioInput address\n");
#endif
				}
			}
          
		}
          catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message with strtok: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }

private :

	int  num_frames;
			int  num_chs;
			int  byte_size;
			int  num_samples ;


		void Get_sensor_Frame( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint, int *num_frames )

	{

		try
		{
			
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                int a1 = (arg++)->AsInt32();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                #ifdef VERBOSE_OSC
                std::cout << "received " << m.AddressPattern() << " message with arguments: "
                    << a1 <<  "\n";
#endif

				*num_frames=a1;
 
		}
		catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while analyzing packet: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }

	}

	void Get_sensor_Rate( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )

	{

		try
		{
			
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                int a1 = (arg++)->AsInt32();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                #ifdef VERBOSE_OSC
                std::cout << "received " << m.AddressPattern() << " message with arguments: "
                    << a1 <<  "\n";
#endif
				
		}
		catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while analyzing packet: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }

	}

	void Get_sensor_Channels( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint, int *num_channels )

	{

		try
		{
			
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                int a1 = (arg++)->AsInt32();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                #ifdef VERBOSE_OSC
                 std::cout << "received " << m.AddressPattern() << " message with arguments: "
                    << a1 <<  "\n";
#endif
				*num_channels=a1;
		}
		catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while analyzing packet: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }

	}

	void Get_sensor_TimeID( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint ,s_int64 *timeid)

	{

		try
		{
			
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                s_int64 a1 = (arg++)->AsInt64();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                #ifdef VERBOSE_OSC
                std::cout << "received " << m.AddressPattern() << " message with arguments: "
                    << a1 <<  "\n";
#endif
				*timeid=a1;
 
		}
		catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while analyzing packet: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }

	}

	void Get_sensor_Data( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint,osc::osc_bundle_element_size_t *size ,float *float_data_buff)

	{

		try
		{
			
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                const void *datap;

				(arg++)->AsBlob(datap,*size);
				memcpy(float_data_buff,datap,*size);
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                #ifdef VERBOSE_OSC
                std::cout << "received " << m.AddressPattern() << " message with first argument: "
                    << float_data_buff[0] <<  "\n";
#endif
 
		}
		catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while analyzing packet: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }

	}

	void Get_sensor_Timestamps( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint,osc::osc_bundle_element_size_t *size ,s_uint64 *int64_data_buff)

	{

		try
		{
			
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                const void *datap;

				(arg++)->AsBlob(datap,*size);
				memcpy(int64_data_buff,datap,*size);
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                #ifdef VERBOSE_OSC
                std::cout << "received " << m.AddressPattern() << " message with first argument: "
                    << int64_data_buff[0] <<  "\n";
#endif
 
		}
		catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while analyzing packet: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }

	}

};