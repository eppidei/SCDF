package it.scdf.framework;

public class Scdf {

	public static final int INVALID = -1;
	public static final int ACCELEROMETER = 0;
	public static final int GYROSCOPE = 1;
	public static final int MAGNETOMETER = 2;
	public static final int PROXIMITY = 3;
	public static final int LIGHT = 4;
	public static final int AUDIOINPUT = 5;
	public static final int NUM_TYPES = 6;
	
	public static final int SENSOR_SETUP_OK = 0;
	public static final int SENSOR_SETUP_OK_CHANGED = 1;
	public static final int SENSOR_SETUP_BROKEN = -1;
	
	public static native boolean IsSensorAvailable(int type);
	public static native int StartSensor(int type);
	public static native int StopSensor(int type);
	public static native int SetupSensor(int type,int rate);
	public static native int SetupAudioInput(int rate, int bufferSize, int channels);
	public static native int GetSensorRate(int type);
	public static native int GetAudioInputBufferSize(); // in samples (divide by chans to obtain frames)
	public static native int GetAudioInputChannels();
	public static native boolean IsSensorActive(int type);
	public static native int StartAllSensors();
	public static native int StopAllSensors();
	public static native int StartPreviouslyActiveSensors();
	
	public static native boolean SetMasterSensor(int type);
	public static native int GetMasterSensor();
	
	public static native void SetUdpDestinationIp(String ip);
	public static native void SetUdpDestinationPort(int port);
	public static native void SetUdpOSCmode(boolean active);
	public static native void SetUdpMultiportMode(boolean active);
	
	public static native String GetUdpDestinationIp();
	public static native int GetUdpDestinationPort();
	public static native boolean IsUdpOSCmodeActive();
	public static native boolean IsUdpMultiportModeActive();
		
	public static native boolean OpenUdpConnection(String ip, int port);
	public static native boolean CloseUdpConnection();
	
	
	public static String TypeStr(int type)
	{
		switch (type) {
			case INVALID: 		return "Invalid";
			case ACCELEROMETER: return "Accelerometer";
			case GYROSCOPE: 	return "Gyroscope";
			case MAGNETOMETER: 	return "Magnetometer";
			case PROXIMITY: 	return "Proximity";
			case LIGHT: 		return "Light";
			case AUDIOINPUT: 	return "Audio Input";
			default: return "Unknown!";
		}
	}
	
}
