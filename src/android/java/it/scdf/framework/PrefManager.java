package it.scdf.framework;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;

public class PrefManager
{
	private static final String TAG = "Preferences Manager";
	
	private static String commonPreferencesFilename = "scdf_shared_preferences";
	private static SharedPreferences commonPreferences = null;
	private static File appCacheDir = null;
		
	public static void Setup(Context ctx)
	{
		commonPreferences = ctx.getSharedPreferences(commonPreferencesFilename,Context.MODE_PRIVATE);
		appCacheDir=ctx.getDir("prefs",Context.MODE_PRIVATE);
	}

	public static String LoadString(String key)
	{
		if (CannotLoad(key)) return "";
		return commonPreferences.getString(key,"");
	}
	
	public static String LoadString(String key, String def)
	{
		if (CannotLoad(key)) return def;
		return commonPreferences.getString(key,def);
	}
	
	public static boolean SaveString(String key, String value) // called from c++
	{
		if (CannotSave(key)) return false;
		return commonPreferences.edit().putString(key, value).commit();
	}
	
	public static int LoadInt(String key,int def)
	{
		if (CannotLoad(key)) return def;
		return commonPreferences.getInt(key,def);
	}
	
	public static boolean SaveInt(String key, int value)
	{
		if (CannotSave(key)) return false;
		return commonPreferences.edit().putInt(key, value).commit();
	}
	
	public static boolean LoadBool(String key,boolean def)
	{
		if (CannotLoad(key)) return def;
		return commonPreferences.getBoolean(key,def);
	}
	
	public static boolean SaveBool(String key, boolean value)
	{
		if (CannotSave(key)) return false;
		return commonPreferences.edit().putBoolean(key, value).commit();
	}
	
	public static boolean SaveObject (Object obj, String filename)
	{
		if (null==appCacheDir) return false;
		File file = new File(appCacheDir,filename);    
		ObjectOutputStream outputStream;
		try {
			outputStream = new ObjectOutputStream(new FileOutputStream(file));
			outputStream.writeObject(obj);
			outputStream.flush();
			outputStream.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		
		return true;
	}
	
	@SuppressWarnings("unchecked") // the type safety is up to the caller... !
	public static <T> T LoadObject (String filename)
	{
		if (null==appCacheDir) return null;
		File file = new File(appCacheDir,filename);    
		ObjectInputStream inputStream = null;
		Object obj = null;
		try {
			inputStream = new ObjectInputStream(new FileInputStream(file));
			obj = inputStream.readObject();
			inputStream.close();
		} catch (FileNotFoundException e) {
			//e.printStackTrace();
			return null;
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			try {
				if (inputStream!=null) inputStream.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			return null;
		}
		return (T)obj;
	}
	
	// Small utilities to check for preferences validity and print a log if not:
	
	private static boolean CannotSave(String key) 
	{
		if (null==commonPreferences) { 
			Log.e(TAG,"Error: can't write "+key+" value. No app preferences yet.");
			return true;
		}
		return false;
	}
	
	private static boolean CannotLoad(String key) 
	{
		if (null==commonPreferences) { 
			Log.e(TAG,"Error: can't write "+key+" value. No app preferences yet.");
			return true;
		}
		return false;
	}
	
}
