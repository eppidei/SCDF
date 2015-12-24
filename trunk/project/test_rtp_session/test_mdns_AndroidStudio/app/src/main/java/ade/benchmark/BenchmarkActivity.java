package main.java.ade.benchmark;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;


public class BenchmarkActivity extends Activity {

	public native void NativeTestmDNS();
	//public native void NativeBenchmark();
	
	static {
		System.loadLibrary("adebenchmark");
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_benchmark);
        Log.d("BENCH",this.getFilesDir().toString());
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.benchmark, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
        case R.id.action_test:
            NativeTestmDNS();
            return true;
        case R.id.action_benchmark:
            NativeTestmDNS();
            return true;
        }
        
        return super.onOptionsItemSelected(item);
    }
}
