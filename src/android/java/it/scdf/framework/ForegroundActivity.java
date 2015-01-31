package it.scdf.framework;

import android.app.Activity;

public class ForegroundActivity {

	static private Activity act = null;
	
	public static  void Set(Activity newForegroundAct)
	{
		act = newForegroundAct;
	}
	
	public static void Detach(Activity detachedAct)
	{
		if (act==detachedAct) act=null;
	}
	
	public static String GetCacheDir()
	{
		return act.getCacheDir().getAbsolutePath();
	}
	  
	public static String GetFilesDir()
	{
		return act.getFilesDir().getAbsolutePath();
	}

	// STORAGE SETUP:
	  // Create a folder into device's external storage (which could actually still be
	  // in the internal device memory) for n-Track projects and files to be saved.
	  // We have 5 possible storage positions:
	  // 1) Application data folder in internal storage, accessible only to the app: 
	  //	getFilesDir() -> /data/data/com.ntrack.studio/files
	  // 2) Internal cache directory. Can be erased by user in device's settings or by the system when needed:
	  //	getCacheDir() -> /data/data/com.ntrack.studio/cache
	  // 3) External files directory, accessible only to the app:
	  //	getExternalFilesDir(TIPO) -> /mnt/sdcard/Android/data/com.ntrack.studio/files
	  // 4) External storage (sdcard or an internal space that acts as the sdcard):
	  //	Environment.getExternalStorageDirectory() -> /mnt/sdcard/
	  // 5) External storage public location (there are several types of it):
	  //	Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MUSIC) -> /mnt/sdcard/Music
	  // NB: lollipop gives new options for the external sd management!

	  
	  
	  /*public String ExternalStorageSetup()
	  {	
		  String desiredPath = null;
		  String state = Environment.getExternalStorageState();

		  if (Environment.MEDIA_MOUNTED.equals(state)) {
			  // setup ntrack dir:
			  desiredPath = Environment.getExternalStorageDirectory()+"/nTrack";
			  File nTrackProjectDir = new File(desiredPath);
			  if (!nTrackProjectDir.exists()) {
				  if (!nTrackProjectDir.mkdir()) {
					  storageState = StorageState.CREATION_FAILED;
					  return getCacheDir().getAbsolutePath();		    	  
			      }
			  }
			  storageState = StorageState.AVAILABLE;
			  return desiredPath;  	    
		  }
		  else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
			  storageState = StorageState.READ_ONLY;
			  return getCacheDir().getAbsolutePath();
		  }
		  else {
			 // alert: can't find sdcard!
			 storageState = StorageState.NOT_FOUND;
			 return getCacheDir().getAbsolutePath();
		  }
	  }*/
	    
	   
	
	
}
