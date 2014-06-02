package it.scdf.test;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;

public class TestActivity extends Activity {

	static {
		System.loadLibrary("scdftest");
	}
	
	private native boolean NativeOnCreate();
	private native boolean NativeOnResume();
	private native boolean NativeOnPause();
	
	private static final String TAG = "Test activity";
	boolean setupOk = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
		setupOk = NativeOnCreate();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
		if (setupOk)
			NativeOnResume();
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
		if (setupOk)
			NativeOnPause();
	}
	
}
