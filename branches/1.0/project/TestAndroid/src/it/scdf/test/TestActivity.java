package it.scdf.test;

import it.scdf.framework.Scdf;
import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.EditText;
import android.widget.Toast;
import android.widget.ToggleButton;

public class TestActivity extends Activity implements OnClickListener, OnEditorActionListener, OnItemSelectedListener
{

	static 
	{
		System.loadLibrary("scdf");
		System.loadLibrary("scdftest");
	}
	
	private native boolean NativeOnCreate();
	private native boolean NativeOnResume();
	private native boolean NativeOnPause();
	private native boolean NativeInit();
	
	private static final String TAG = "Test activity";
	private static final String APP_PREF_FILE = "com_scdf_test_";
	
	boolean setupOk = false;
	
	private int TypeToPanelId(int type)
	{
		switch (type) {
			case Scdf.INVALID: 		return 0;
			case Scdf.ACCELEROMETER:return R.id.accelerometer_panel;
			case Scdf.GYROSCOPE: 	return R.id.gyroscope_panel;
			case Scdf.MAGNETOMETER: return R.id.magnetometer_panel;
			case Scdf.PROXIMITY: 	return R.id.proximity_panel;
			case Scdf.LIGHT: 		return R.id.light_panel;
			case Scdf.AUDIOINPUT:	return R.id.audio_in_panel;
			default: 				return 0;
		}
	}

	private View Find(int id)
	{
		return findViewById(id);
	}
	
	private View Find(int type, int id)
	{
		View panel = findViewById(TypeToPanelId(type));
		if (panel==null) return null;
		return panel.findViewById(id);
	}
	
	private void SetupStandardSensorPanel(int type)
	{
		if (!Scdf.IsSensorAvailable(type)) {
			Find(TypeToPanelId(type)).setVisibility(View.GONE);
			return;
		}
		Find(type,R.id.toggle).setOnClickListener(this);
		Find(type,R.id.toggle).setTag(type);
		((EditText)Find(type,R.id.rate)).setOnEditorActionListener(this);
		Find(type,R.id.rate).setTag(type);
		((TextView)Find(type,R.id.text)).setText(Scdf.TypeStr(type));
	}
	
	private void SetupAudioInputPanel() 
	{
		Find(Scdf.AUDIOINPUT,R.id.toggle).setOnClickListener(this);
		Find(Scdf.AUDIOINPUT,R.id.toggle).setTag(Scdf.AUDIOINPUT);
		((EditText)Find(Scdf.AUDIOINPUT,R.id.buffer_size)).setOnEditorActionListener(this);
		Find(Scdf.AUDIOINPUT,R.id.buffer_size).setTag(Scdf.AUDIOINPUT);
		
		ArrayAdapter<Integer> adapter = new ArrayAdapter<Integer>(this, android.R.layout.simple_spinner_item);
		adapter.add(8000);
		adapter.add(16000);
		adapter.add(32000);
		adapter.add(44100);
		adapter.add(48000);
		((Spinner)Find(Scdf.AUDIOINPUT,R.id.spinner_rate)).setAdapter(adapter);
		((Spinner)Find(Scdf.AUDIOINPUT,R.id.spinner_rate)).setOnItemSelectedListener(this);
	}
	
	private void SetupGeneralSettingsPanel()
	{
		((EditText)Find(R.id.edittext_ip)).setOnEditorActionListener(this);
		((EditText)Find(R.id.edittext_port)).setOnEditorActionListener(this);
		
		Find(R.id.toggle_connect).setOnClickListener(this);
		Find(R.id.toggle_osc).setOnClickListener(this);
		Find(R.id.toggle_multiport).setOnClickListener(this);
	}
	
	void SetupViews()
	{
		SetupGeneralSettingsPanel();
		SetupAudioInputPanel();
		SetupStandardSensorPanel(Scdf.ACCELEROMETER);
		SetupStandardSensorPanel(Scdf.GYROSCOPE);
		SetupStandardSensorPanel(Scdf.MAGNETOMETER);
		SetupStandardSensorPanel(Scdf.PROXIMITY);
		SetupStandardSensorPanel(Scdf.LIGHT);
	}
	
	private void RefreshPanel(int type)
	{
		if (!Scdf.IsSensorAvailable(type))
			return;
		
		boolean act = Scdf.IsSensorActive(type);
		int rate = Scdf.GetSensorRate(type);
		if (type!=Scdf.AUDIOINPUT)
			((EditText)Find(type,R.id.rate)).setText(Integer.toString(rate));
		((ToggleButton)Find(type,R.id.toggle)).setChecked(act);
	}
	
	void RefreshViews()
	{
		// general:
		((EditText)Find(R.id.edittext_ip)).setText( Scdf.GetUdpDestinationIp() );
		((EditText)Find(R.id.edittext_port)).setText( Integer.toString(Scdf.GetUdpDestinationPort()) );
		((ToggleButton)Find(R.id.toggle_multiport)).setChecked( Scdf.IsUdpMultiportModeActive() );
		((ToggleButton)Find(R.id.toggle_osc)).setChecked( Scdf.IsUdpOSCmodeActive() );
		//((Spinner)Find(R.id.spinner_mastersensor)).setTag(true); // ignore this setting!
		
		// TODO: refresh state of connection, like: ((ToggleButton)Find(R.id.toggle_connection)).setChecked( isconnected? );
		
		// audio:
		((EditText)Find(Scdf.AUDIOINPUT,R.id.buffer_size)).setText( Integer.toString(Scdf.GetAudioInputFramesPerBuffer()) );
		((ToggleButton)Find(Scdf.AUDIOINPUT,R.id.toggle)).setChecked( Scdf.IsSensorActive(Scdf.AUDIOINPUT) );
		Spinner rs = ((Spinner)Find(R.id.spinner_rate));
		for (int i=0; i<rs.getAdapter().getCount(); i++) {
			if ( ((Integer)rs.getAdapter().getItem(i))==Scdf.GetSensorRate(Scdf.AUDIOINPUT)) {
				//((Spinner)Find(R.id.spinner_rate)).setTag(true); // ignore this setting!
				rs.setSelection(i);
				break;
			}
		}
		
		// sensors:
		for (int i=0; i<Scdf.AUDIOINPUT; i++)
			RefreshPanel(i);
	}
	
	void SavePreferences()
	{
		SharedPreferences p = getSharedPreferences(APP_PREF_FILE, Context.MODE_PRIVATE);
		SharedPreferences.Editor ed = p.edit();
		Log.i(TAG,"Save ip : "+Scdf.GetUdpDestinationIp());
		ed.putString("DEST_IP", Scdf.GetUdpDestinationIp());
		ed.putInt("DEST_PORT",Scdf.GetUdpDestinationPort());
		ed.putBoolean("MULTIPORT_MODE", Scdf.IsUdpMultiportModeActive());
		ed.putBoolean("OSC_MODE", Scdf.IsUdpOSCmodeActive() );
		//ed.putBoolean("UDP_CONNECTED", ((ToggleButton)Find(R.id.toggle_connect)).isChecked());
		// TODO: better way to retrieve connection state
		
		
		ed.putInt(Scdf.TypeStr(Scdf.AUDIOINPUT)+"_BUFFERSIZE", Scdf.GetAudioInputFramesPerBuffer() );
				
		for (int i=0; i<Scdf.NUM_TYPES; i++) {
			if (!Scdf.IsSensorAvailable(i))
				continue;
			ed.putInt(Scdf.TypeStr(i)+"_RATE", Scdf.GetSensorRate(i) );
			//ed.putBoolean(Scdf.TypeStr(i)+"_ACTIVE", Scdf.IsSensorActive(i) );
		}
		ed.commit();
	}
	
	void LoadPreferences()
	{
		SharedPreferences p = getSharedPreferences(APP_PREF_FILE, Context.MODE_PRIVATE);
		
		String ip = p.getString("DEST_IP", Scdf.GetUdpDestinationIp());
		Log.i(TAG,"loaded ip: "+ip);
		
		int port = p.getInt("DEST_PORT",Scdf.GetUdpDestinationPort());
		boolean multiport = p.getBoolean("MULTIPORT_MODE", Scdf.IsUdpMultiportModeActive());
		boolean osc = p.getBoolean("OSC_MODE", Scdf.IsUdpOSCmodeActive() );
		
		//boolean conn = p.getBoolean("UDP_CONNECTED", ((ToggleButton)Find(R.id.toggle_connect)).isChecked());
		
		//((EditText)Find(R.id.edittext_ip)).setText(ip);
		//((EditText)Find(R.id.edittext_port)).setText(Integer.toString(port));
		Scdf.SetUdpOSCmode(osc);
		Scdf.SetUdpMultiportMode(multiport);
		Scdf.OpenUdpConnection(ip,port);
		Scdf.CloseUdpConnection();
		
		int audioBufSize = p.getInt(Scdf.TypeStr(Scdf.AUDIOINPUT)+"_BUFFERSIZE", Scdf.GetAudioInputFramesPerBuffer() );
		int audiorate = p.getInt(Scdf.TypeStr(Scdf.AUDIOINPUT)+"_RATE", Scdf.GetSensorRate(Scdf.AUDIOINPUT) );
		//boolean audioactive = p.getBoolean(Scdf.TypeStr(Scdf.AUDIOINPUT)+"_ACTIVE", Scdf.IsSensorActive(Scdf.AUDIOINPUT) );
		Scdf.SetupAudioInput(audiorate, audioBufSize, 1);
		
		// TODO: load audio in channels
		
		for (int i=0; i<Scdf.AUDIOINPUT; i++) {
			if (!Scdf.IsSensorAvailable(i))
				continue;
			int rate = p.getInt(Scdf.TypeStr(i)+"_RATE", Scdf.GetSensorRate(i) );
			Scdf.SetupSensor(i,rate);
			//p.getBoolean(Scdf.TypeStr(i)+"_ACTIVE", Scdf.IsSensorActive(i) );
		}
		
		RefreshViews();
		((EditText)Find(R.id.edittext_ip)).setText( "79.23.90.225" );
		((EditText)Find(R.id.edittext_port)).setText( Integer.toString(50000) );
		
		
	}
		
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
		setupOk = NativeOnCreate();
		SetupViews();
		//RefreshViews();	
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
		if (setupOk)
			NativeOnResume();
		
		LoadPreferences();
		Scdf.StartPreviouslyActiveSensors();
		
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
		if (setupOk)
			NativeOnPause();
		
		Scdf.StopAllSensors();
		SavePreferences();
		//Scdf.CloseUdpConnection();
		// TODO: find clever way to stop udp connection
		// while unchecking the connection toggle
	}
	
	int TypeFromTag(View v)
	{
		return ((Integer)v.getTag()).intValue();
	}
	
	@Override
	public void onClick(View v)
	{
		switch (v.getId()) {
		
		case R.id.toggle:
			OnSensorStateToggle((ToggleButton)v);
			break;
		case R.id.toggle_connect:
			OnConnectToggle((ToggleButton)v);
			break;
		case R.id.toggle_multiport:
			OnMultiportToggle((ToggleButton)v);
			break;
		case R.id.toggle_osc:
			OnOSCToggle((ToggleButton)v);
			break;
		default:
			Log.e(TAG,"Unandled click");
		}
		//NativeInit();
	}
	
	void OnSensorStateToggle(ToggleButton t)
	{
		int type = ((Integer)t.getTag()).intValue();
		if (t.isChecked()) {
			if ( !Scdf.StartSensor(type) ) {
				t.setChecked(false);
				QuickAlert("Could not start "+Scdf.TypeStr(type));
			}
		}
		else {
		
			if ( !Scdf.StopSensor(type)) {
				QuickAlert("Error stopping "+Scdf.TypeStr(type));
			}
		}
		RefreshPanel(type);
	}
	
	private void OnConnectToggle(ToggleButton t)
	{
		if (t.isChecked()) {
		
			String ip = ((EditText)Find(R.id.edittext_ip)).getText().toString();
			if (ip.isEmpty()) {
				QuickAlert("Provide a valid destination IP");
				t.setChecked(false);
				return;
			}
			
			int port = TextToInt((TextView)Find(R.id.edittext_port));
			if (port == 0) {
				QuickAlert("Provide a valid destination port");
				t.setChecked(false);
				return;
			}
			
			if (!Scdf.OpenUdpConnection(ip,port)) {
				QuickAlert("Error opening UDP connection");
				t.setChecked(false);
			}
			
		} else {
			if (!Scdf.CloseUdpConnection())
				QuickAlert("Error closing UDP connection");
		}
		RefreshViews();
	}
	
	private void OnMultiportToggle(ToggleButton v)
	{
		Scdf.SetUdpMultiportMode(v.isChecked());
	}
	
	private void OnOSCToggle(ToggleButton v)
	{
		Scdf.SetUdpOSCmode(v.isChecked());
	}
	
	int TextToInt(TextView tv) 
	{
		int res;
		try {
			res = Integer.parseInt( tv.getText().toString() );
		}
		catch( NumberFormatException nfe) {
			Toast("Invalid number in edit text");
			res = 0;
		}
		return res;
	}
	
	@Override
	public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
	{
		switch (v.getId()) {
		case R.id.rate:
			OnSensorRateSelected(TypeFromTag(v),TextToInt(v));
			break;
		case R.id.buffer_size:
			OnBufferSizeSelected(v);
			break;
		}
		return false;
	}
	
	private void OnBufferSizeSelected(TextView v)
	{
		int type = ((Integer)v.getTag()).intValue();
		int res = Scdf.SetupAudioInput(-1, TextToInt(v) , -1);
		// NB: -1 for rate and chan -> don't touch them
		if (Scdf.SENSOR_SETUP_BROKEN == res) {
			QuickAlert("Error setting buffer size for "+Scdf.TypeStr(type));
		}
		RefreshPanel(type);
	}
	
	private void OnSensorRateSelected(int type, int value)
	{
		int res = type == Scdf.AUDIOINPUT ? Scdf.SetupAudioInput(value, -1, -1) : Scdf.SetupSensor(type,value);
		if (Scdf.SENSOR_SETUP_BROKEN == res) {
			QuickAlert("Error setting new rate for "+Scdf.TypeStr(type));
		} else if (Scdf.SENSOR_SETUP_OK_CHANGED == res) {
			Toast(Scdf.TypeStr(type)+" rate adjusted to supported value");
		}
		RefreshPanel(type);
	}
	
	@Override
	public void onItemSelected(AdapterView<?> adapt, View v, int pos, long id)
	{
		if (adapt.getTag()!=null) {
			if ( ((Boolean)adapt.getTag()).booleanValue()==true ) // asked to ignore this
					return;
		}
		
		switch(adapt.getId()) {
			
			case R.id.spinner_rate:
			{
				int rate = (Integer)adapt.getAdapter().getItem(pos);
				Toast("Audio rate selection. rate "+rate);
				Scdf.SetupAudioInput(rate, -1, -1);
			}
			break;
			
			default:
				Log.e(TAG,"Unknown spinner");
			
		}
		
	}
	
	@Override
	public void onNothingSelected(AdapterView<?> adapt) {}
	
	
	
	private void QuickAlert(String msg)
	{
		new AlertDialog.Builder(this).setMessage(msg).setNeutralButton("Got it", null).create().show();
	}
	
	private void Toast(String msg)
	{
		Toast.makeText(this,msg,android.widget.Toast.LENGTH_LONG).show();
	}
	
}
