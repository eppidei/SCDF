package it.scdf.test;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;

public class TestActivity extends Activity implements OnClickListener {

	static {
		System.loadLibrary("scdftest");
	}
	
	private native boolean NativeOnCreate();
	private native boolean NativeOnResume();
	private native boolean NativeOnPause();
	private native boolean NativeInit();
	
	private static final String TAG = "Test activity";
	boolean setupOk = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
		setupOk = NativeOnCreate();
		findViewById(R.id.button_init).setOnClickListener(this);
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
	@Override
	public void onClick(View arg0) {
		NativeInit();
	}
	
}
