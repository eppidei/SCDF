package it.scdf.framework;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

@SuppressLint("NewApi") 
public class UsbHandler {
		
	public static final int ITF_SUBCLASS_AUDIOCONTROL = 1;
	public static final int ITF_SUBCLASS_AUDIOSTREAMING = 2;
	public static final int ITF_SUBCLASS_MIDISTREAMING = 3;
	// as for the usb 2.0 standard.
	
	private static boolean hasUsbHostFeature = false;
	
	public static boolean IsUsbSupported()
	{
		return (android.os.Build.VERSION.SDK_INT>=12 && hasUsbHostFeature); // HONEYCOMB_MR1 (3.1)
	}
	
	public static final String TAG = "Usb Handler";
	
	private static UsbManager manager = null;
	private static PendingIntent usbIntent;
	private static final String ACTION_USB_PERMISSION = "com.scdf.USB_PERMISSION";
	
	public static class UsbDeviceBundle {
		public UsbDevice device = null;
		public UsbDeviceConnection connection = null;
		public long nativeHandle = 0;
		//public scdfAndroidMidiInput midiInput = null;
	}
		
	//private static Map<Integer,UsbDeviceBundle> openDevices = new LinkedHashMap<Integer,UsbDeviceBundle>(); 
	private static ArrayList<UsbDeviceBundle> openDevices = new ArrayList<UsbDeviceBundle>(); 


	// this map contains unique usb devices ids paired with all the objects describing the device
	// (java and native part) and opened connections.
	private static Integer lastOpenedDeviceId = 0;
	static Integer GetNewOpenedDeviceId() { lastOpenedDeviceId++; return lastOpenedDeviceId;}
	
	
	/* Open the native usb device using libusb */	
	private static native long NativeOpenDevice(int fd, int vid, int pid);
	/* Close the native usb device using libusb */	
	private static native void NativeCloseDevice(long nativeDevPtr);
	/* Notify native entities that asked to know about usb events */	
	private static native void CallNativeListeners(int deviceId, int evCode);
			
	public static int GetNumOpenDevices()
	{
		return openDevices.size();
	}
	
	public static long GetDevice(int index)
	{
		Log.d(TAG,"USB - get device (java): "+index+". openDevices: "+openDevices.size());
		if (index >= openDevices.size()) return 0;
		return openDevices.get(index).nativeHandle;
	}
	
	
	private final static BroadcastReceiver usbReceiver = new BroadcastReceiver()
	{
	    public void onReceive(Context context, Intent intent)
	    {
	    	Log.i(TAG,"USB receiver - Receiver received something...");
	    	
	    	String action = intent.getAction();
	        if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
	        	Log.i(TAG,"USB - action usb device detached");
	    		UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
	    		if (device != null) {
	    			CloseUsbDevice(device);
	    		}
	    		Log.i(TAG, "USB - usb device parcelable extra got");
	    	}
	        else if (ACTION_USB_PERMISSION.equals(action)) {
	            Log.i(TAG," USB - onReceive: ACTION_USB_PERMISSION");
	            synchronized (this) {
	                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
	                if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
	                    if(device != null){
	                        Log.i(TAG," Permission granted for device "+device.getVendorId());
	                        OpenUsbDevice(device);
	                    }
	                }
	                else {
	                    Log.e(TAG," USB - Permission denied for device " + device.getVendorId());
	                }
	            }
	        }
	    }
	};
	
	public static boolean Setup(Activity act)
	{
		Log.d(TAG,"USB - Handler Setup");
		hasUsbHostFeature = act.getPackageManager().hasSystemFeature("android.hardware.usb.host");
		if (!IsUsbSupported()) return false;
		
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
		//filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED); // won't work. needs to be declared in manifest
		act.registerReceiver(usbReceiver, filter);
		usbIntent = PendingIntent.getBroadcast(act, 0, new Intent(ACTION_USB_PERMISSION), 0);
		
		manager = (UsbManager) act.getSystemService(Context.USB_SERVICE);
		Log.d(TAG,"USB - Requested usb manager. got: " + manager);
		return true;
	}
		
	public static void Detach(Activity act)
	{
		act.unregisterReceiver(usbReceiver);
	}
	
	public static void Dismiss(Activity act) 
	// NB: don't call this if you know you are going to re-create the activity or,
	// in general, if you don't want to be forced to close and recreate USB connections
	// when switching to another activity. Prefer Detach(Activity act).
	{
		Log.d(TAG,"Dismiss Usb handler");
		Detach(act);
		
		//Set<Integer> devices = openDevices.keySet();
		for (int i=openDevices.size(); i>=0; i--)
			CloseUsbDevice(i);
		
		//ClearListeners();
		
		manager = null;
		usbIntent = null;
	}
	
	/**
	 *  this is needed before opening, to let scdf discover useful devices:
	 */	
	public static boolean HasAudioCapabilities(UsbDevice dev) // may be midi or audio (or both!)
	{
		for(int i=0; i < dev.getInterfaceCount(); i++) {
			UsbInterface itf = dev.getInterface(i);
	    	if (itf.getInterfaceClass() == UsbConstants.USB_CLASS_AUDIO) {
	    		Log.i(TAG,"USB - Found device with audio capabilities");		
	    		return true;
	    	}
		}
	    return false;
	}
		
	public static UsbDevice FindAudioDevice() // finds the first audio or midi device
	{
		if (null==manager) {
			Log.e(TAG,"USB - Cannot look for usb audio device. manager is not set-up");
			return null;
		}
		
		HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
				
		Iterator<UsbDevice> deviceIterator = deviceList.values().iterator();
		if (deviceList.isEmpty()) {
			Log.e(TAG,"USB - No device connected");
			return null;
		}
		while(deviceIterator.hasNext()){
		    UsbDevice d = deviceIterator.next();
		    if ( HasAudioCapabilities(d) ) {
		    	Log.i(TAG, "USB - device with audio capabilities found!");
		    	return d;
		    }
		}
		return null;
	}
	
	public static boolean TryOpeningDevice(UsbDevice dev)
	{
		Log.i(TAG,"USB - Try Opening device : "+ dev);
		
		if (null==dev) {
			Log.i(TAG,"USB - no device to open!");
			return false;
		}
		
		if (!manager.hasPermission(dev)) {
			Log.i(TAG,"USB - No permission to access device. Requesting it.");
			manager.requestPermission(dev, usbIntent);
			Log.i(TAG,"USB - requested permission to manager");
			return false;
		} else {
			Log.i(TAG,"USB - Already have permission for device ");
			OpenUsbDevice(dev);
			return true;
		}
	}	
		
	public static boolean TryOpeningFirstUsbDevice()
	{
		Log.i(TAG,"USB  - try opening first usb device!");
		return TryOpeningDevice(FindAudioDevice());
	}
	
	
	public static boolean RecognizeAndHandleUsbAttachIntent(Intent intent)
	{
		Log.d(TAG,"USB - recognize and handle usb attach intent");
		UsbDevice device = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE); 
	    if (device != null && HasAudioCapabilities(device)) { 
	    	Log.d(TAG,"Received an USB attached intent: try opening device!");
	    	TryOpeningDevice(device); // may return false, but see below...
	    	return true;
	    	// returns true even if it didn't open device, to signal that the intent had 
	    	// an usb device attached to it and so the permission (if not already granted 
	    	// as it should be) has been asked for that particular device, so no need to
	    	// scan connected devices manually
	    }
	    Log.d(TAG,"No USB device attachment info in intent");
	    return false;
	}
	
		
	public static  int NumOpenedDevices() { return openDevices.size(); }
			
	private static void OpenUsbDevice(UsbDevice dev)
	{
		Log.i(TAG,"USB - Open USB device (java) :" +dev);
		
		if (dev==null) { 
			Log.e(TAG,"USB - No audio device to open");
			UsbHandler.CallListeners(dev.getDeviceId(), UsbHandler.DEVICE_OPEN_ERROR);
			return;
		}
		
		for (UsbDeviceBundle b : openDevices) {
			if (b.device.getDeviceId() == dev.getDeviceId()) {
				Log.w(TAG, "USB - Device already opened!");
				return;
			}
		}
		
		UsbDeviceBundle usbBundle = new UsbDeviceBundle();
		usbBundle.device = dev;
		usbBundle.connection = manager.openDevice(dev);
		Log.i(TAG,"USB - Usb connection after opendevice: "+usbBundle.connection);
		
		if (usbBundle.connection==null) {
			Log.e(TAG,"USB - Couldn't open connection with usb device");
			UsbHandler.CallListeners(usbBundle.device.getDeviceId(), UsbHandler.DEVICE_OPEN_ERROR);
			return;
		}
		
		// apri device usando libusb
		
		int fd = usbBundle.connection.getFileDescriptor();
		Log.i(TAG,"USB - Opened device. File descriptor: "+fd);
		usbBundle.nativeHandle = NativeOpenDevice(fd,dev.getVendorId(),dev.getProductId());
			
		if (0==usbBundle.nativeHandle) {
			Log.e(TAG,"USB - Something went wrong in opening usb device on native side");
			usbBundle.connection.close();
			UsbHandler.CallListeners(usbBundle.device.getDeviceId(), UsbHandler.DEVICE_OPEN_ERROR);
			return;
		}
		
		//openDevices.put( GetNewOpenedDeviceId() , usbBundle );
		openDevices.add( usbBundle );
		UsbHandler.CallListeners(usbBundle.device.getDeviceId(), UsbHandler.DEVICE_OPEN_SUCCESS);
	}
	
	/*public static long GetDevicePtr(long id)
	{
		if (!openDevices.containsKey(id)) 
			return 0;
		return openDevices.get(id).nativeHandle;
	}*/	
			
	private static void CloseUsbDevice(int index)
	{
		Log.d(TAG,"USB - Close usb device (java part)");
		
		if (openDevices.size()<=index)	return;
		UsbDeviceBundle ub = openDevices.get(index);
		
		if (0!=ub.nativeHandle) {
			Log.d(TAG,"USB - Close native audio streaming device");
			NativeCloseDevice( ub.nativeHandle );
		}
				
		ub.connection.close();
		openDevices.remove(index);
		UsbHandler.CallListeners(ub.device.getDeviceId(), UsbHandler.DEVICE_DISCONNECTED);
	}
	
	private static void CloseUsbDevice(UsbDevice device)
	{
		Log.d(TAG,"USB - Close usb device using UsbDevice (java part)");
		
		for (int i=openDevices.size()-1; i>=0; i--)
		{
			if (openDevices.get(i).device==device)
				CloseUsbDevice(i);
		}
	}
	
	/**
	 * 		LISTENERS:
	 */
	
	public static final int DEVICE_UNSUPPORTED = -1;
	public static final int DEVICE_DISCONNECTED = -2;	
	public static final int STREAMING_ERROR = -3;
	public static final int DEVICE_OPEN_ERROR = -4;
	public static final int DEVICE_OPEN_SUCCESS = 0;
	
	public static interface UsbListener {
		public abstract void OnUsbEvent(int deviceId, int eventCode);
		//public abstract void OnUsbMidiEvent(scdfAndroidMidiInput midiIn, int eventCode);
	}

	private static Set<UsbListener> listeners = new HashSet<UsbListener>();
	public static boolean AddListener(UsbListener l) { return listeners.add(l); }
	public static boolean RemoveListener(UsbListener l) { return listeners.remove(l); }
	public static void ClearListeners() { listeners.clear();}
	
	private static void CallListeners(final int deviceId, final int evCode) 
	{ 
		new Handler(Looper.getMainLooper()).post( new Runnable() {
		    @Override 
		    public void run() 
		    {
				Log.i(TAG,"USB - java handler call java listeners");
				// java listeners:
				for (UsbListener list: listeners) 
					list.OnUsbEvent(deviceId,evCode);
				// native listeners:
				Log.i(TAG,"USB java handler- call native listeners");
				CallNativeListeners(deviceId,evCode);
		    }
		});
	}
	
	// Will need something like this in the activity for auto discovery of usb devices:
	
	/*  @Override
	protected void onNewIntent(Intent intent) {
	    super.onNewIntent(intent);
	    
	    if (null!= usbHandler && UsbHandler.IsUsbSupported())
	    	usbHandler.RecognizeAndHandleUsbAttachIntent(intent);
	   
	    // getIntent() should always return the most recent:
	    setIntent(intent);
	}	
  
   
   */
	
	
	
	
}