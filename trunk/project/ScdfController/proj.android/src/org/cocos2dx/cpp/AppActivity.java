/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import it.scdf.controller.PurchaseManager;
import it.scdf.controller.PurchaseManager.PurchaseListener;
import it.scdf.framework.PrefManager;
import it.scdf.framework.UsbHandler;
import it.scdf.framework.ForegroundActivity;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class AppActivity extends Cocos2dxActivity implements PurchaseListener {
	
	static
	{
		System.loadLibrary("usbcustom");
		System.loadLibrary("blas");
		System.loadLibrary("fftw3");
		System.loadLibrary("fftw_threads");
		System.loadLibrary("ade");
		System.loadLibrary("scdf");
		Log.i("sc","SCDF native libraries loaded");
	}
	
	private static native boolean SetupScdfSensorAPI();
	
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Log.i("App activity","USB - activity on crete");
        ForegroundActivity.Set(this);
        SetupScdfSensorAPI();
        UsbHandler.Setup(this);
        PrefManager.Setup(this);
        PurchaseManager.Get().StartSetup(this,this);
        //UsbHandler.TryOpeningFirstUsbDevice();
        
        /*if (getIntent()!=null) {
        	Log.d("App activity","USB - intent in oncreate...");
        	UsbHandler.RecognizeAndHandleUsbAttachIntent(getIntent());
        }*/
    }
    
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        Log.i("App activity","USB - activity on destroy");
        ForegroundActivity.Detach(this);
        PurchaseManager.Get().Dispose();
        UsbHandler.Detach(this);
    }
    
    /*@Override
	protected void onNewIntent(Intent intent)
    {
    	Log.i("App activity","USB - on new intent");
	   if (!UsbHandler.RecognizeAndHandleUsbAttachIntent(intent))
		   super.onNewIntent(intent);
	    // getIntent() should always return the most recent:
	    setIntent(intent);
	}*/	
    
    @Override
    public Cocos2dxGLSurfaceView onCreateView()
    {
    	Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
    	glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
    	return glSurfaceView;
    } 
    
    @Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (PurchaseManager.Get().HandleActivityResult(requestCode, resultCode, data)) {
			Log.d("scdfctrl activity","activity result was related to billing and has been handled accordingly");
			return;
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	public void OnPurchaseFailed(String itemName, String message)
	{
		AlertDialog.Builder bld = new AlertDialog.Builder(this);
		bld.setTitle("Purchase Error").setMessage("Purchase failed for item: "+itemName+"\n"+message);
		bld.setNegativeButton("Ok",null);
		bld.create().show();
	}

	@Override
	public void OnItemPurchaseStateChange(String itemName, int state)
	{
		/*if (state!=PurchaseManager.PURCHASED) return;
		AlertDialog.Builder bld = new AlertDialog.Builder(this);
		bld.setTitle("Purchase success").setMessage("Item "+itemName+"has been purchased!\nYou may have to restart the app to see the changes");
		bld.setNegativeButton("Ok",null);
		bld.create().show();*/
	}
}
