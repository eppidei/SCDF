package com.scdf.rtptest;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener {

	static {
		System.loadLibrary("RTPTest");
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		findViewById(R.id.btn_test_A).setOnClickListener(this);
		findViewById(R.id.btn_test_B).setOnClickListener(this);
		findViewById(R.id.btn_test_C).setOnClickListener(this);
	}
	
	private native String TestA();
	private native String TestB();
	private native String TestC();

	@Override
	public void onClick(View v) {
		
		String res = "";
		TextView tv =((TextView)findViewById(R.id.text_view));
		
		switch (v.getId())
		{
			case R.id.btn_test_A: 
				tv.setText("Test A...");
				res = TestA();
				break;
			case R.id.btn_test_B:
				tv.setText("Test B...");
				res = TestB();
				break;
			case R.id.btn_test_C:
				tv.setText("Test C...");
				res = TestC();
				break;
				
		}
		
		tv.setText(res);
				
	}
		
}
