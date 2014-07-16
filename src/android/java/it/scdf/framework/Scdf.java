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
