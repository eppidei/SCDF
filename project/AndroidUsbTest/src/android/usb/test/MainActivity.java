package android.usb.test;

import it.scdf.framework.UsbHandler;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;



public class MainActivity extends Activity implements OnClickListener {

	static {
		System.loadLibrary("usbcustom");
		System.loadLibrary("scdfusbtest");
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.button1).setOnClickListener(this);
        UsbHandler.Setup(this);
    }
    
    public static final String TAG = "Android USB test";
    void Toast(String msg) { Toast.makeText(this, msg, Toast.LENGTH_LONG).show();}
	
    @Override
	public void onClick(View v) 
    {
    	boolean res = UsbDiscoverTest();
    	Toast("Test discover res : "+res);
	}
    
    public native boolean UsbDiscoverTest();
    
}
