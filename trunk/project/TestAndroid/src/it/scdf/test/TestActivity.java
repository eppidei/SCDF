package it.scdf.test;

import it.scdf.framework.Scdf;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.ToggleButton;
import android.widget.EditText;;

public class TestActivity extends Activity implements OnClickListener, OnEditorActionListener, OnItemSelectedListener {

	static {
		System.loadLibrary("scdftest");
	}
	
	private native boolean NativeOnCreate();
	private native boolean NativeOnResume();
	private native boolean NativeOnPause();
	private native boolean NativeInit();
	
	private static final String TAG = "Test activity";
	boolean setupOk = false;
	

	View Find(int id)
	{
		return findViewById(id);
	}
	
	View Find(int parentId, int id)
	{
		return findViewById(parentId).findViewById(id);
	}
	
	void SetupStandardSensorPanel(int id, int type)
	{
		Find(id,R.id.toggle).setOnClickListener(this);
		Find(id,R.id.toggle).setTag(type);
		((EditText)Find(id,R.id.rate)).setOnEditorActionListener(this);
		Find(id,R.id.rate).setTag(type);
		((TextView)Find(id,R.id.text)).setText(Scdf.TypeStr(type));
	}
	
	private void SetupAudioInputPanel() 
	{
		Find(R.id.audio_in_panel,R.id.toggle).setOnClickListener(this);
		Find(R.id.audio_in_panel,R.id.toggle).setTag(Scdf.AUDIOINPUT);
		((EditText)Find(R.id.audio_in_panel,R.id.buffer_size)).setOnEditorActionListener(this);
		Find(R.id.audio_in_panel,R.id.buffer_size).setTag(Scdf.AUDIOINPUT);
		
		ArrayAdapter<Integer> adapter = new ArrayAdapter<Integer>(this, android.R.layout.simple_spinner_item);
		adapter.add(8000);
		adapter.add(16000);
		adapter.add(32000);
		adapter.add(44100);
		adapter.add(48000);
		((Spinner)Find(R.id.audio_in_panel,R.id.rate)).setAdapter(adapter);
		((Spinner)Find(R.id.audio_in_panel,R.id.rate)).setOnItemSelectedListener(this);
	}
	
	private void SetupGeneralSettingsPanel()
	{
		((EditText)Find(R.id.edittext_ip)).setOnEditorActionListener(this);
		((EditText)Find(R.id.edittext_port)).setOnEditorActionListener(this);
		
		Find(R.id.toggle_connect).setOnClickListener(this);
		Find(R.id.toggle_osc).setOnClickListener(this);
		Find(R.id.toggle_multiport).setOnClickListener(this);
		
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item);
		for (int i=0; i<Scdf.NUM_TYPES; i++)
			adapter.add(Scdf.TypeStr(i));
		((Spinner)Find(R.id.spinner_mastersensor)).setAdapter(adapter);
		((Spinner)Find(R.id.spinner_mastersensor)).setOnItemSelectedListener(this);	
	}
	
	void SetupViews()
	{
		SetupGeneralSettingsPanel();
		SetupAudioInputPanel();
		SetupStandardSensorPanel(R.id.accelerometer_panel,Scdf.ACCELEROMETER);
		SetupStandardSensorPanel(R.id.gyroscope_panel,Scdf.GYROSCOPE);
		SetupStandardSensorPanel(R.id.magnetometer_panel,Scdf.MAGNETOMETER);
		SetupStandardSensorPanel(R.id.proximity_panel,Scdf.PROXIMITY);
		SetupStandardSensorPanel(R.id.light_panel,Scdf.LIGHT);
	}
		
	void RefreshViews()
	{
		
	}
	
	void SavePreferences()
	{
		
	}
	
	void LoadPreferences()
	{
		
	}
		
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
		SetupViews();
		
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
	
	@Override
	public void onClick(View v)
	{
		NativeInit();
	}
	
	void Toggle(View v,boolean on)
	{
		((ToggleButton)v).setChecked(on);
	}
	
	@Override
	public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
	{
		
		return false;
	}
	
	@Override
	public void onItemSelected(AdapterView<?> adapt, View v, int id, long sel)
	{
		
		
	}
	
	@Override
	public void onNothingSelected(AdapterView<?> adapt) {}
	
}
