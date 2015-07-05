package it.scdf.controller;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

import it.scdf.controller.IabHelper;
import it.scdf.controller.IabResult;
import it.scdf.controller.Inventory;
import it.scdf.controller.Purchase;
import it.scdf.controller.SkuDetails;
import it.scdf.framework.PrefManager;


public class PurchaseManager 
	implements IabHelper.OnIabSetupFinishedListener,
			IabHelper.QueryInventoryFinishedListener,
			IabHelper.OnIabPurchaseFinishedListener
{

	public abstract interface PurchaseListener {
		public abstract void OnPurchaseFailed(String itemName,String message);
		public abstract void OnItemPurchaseStateChange(String itemName, int state);
	}
	
	// Purchase manager static methods (to be called from c++ side):
	
	public static void StartPurchase(int id)
	{
		String name = NativeIdToItemName(id);
		if (name.isEmpty()) {
			LOGE("Start purchase error - no item name for id: "+id);
			return;
		}
		Get().StartItemPurchase(name);
	}
	
	public static boolean IsItemPurchased(int id)
	{
		String name = NativeIdToItemName(id);
		if (name.isEmpty()) {
			LOGE("Purchase state error - no item name for id: "+id);
			return false;
		}
		return Get().IsItemPurchased(name);
	}
	
	public static boolean IsItemPurchasedShowInvite(int id)
	{
		String name = NativeIdToItemName(id);
		if (name.isEmpty()) {
			LOGE("Purchase state error - no item name for id: "+id);
			return false;
		}
		boolean purchased =  Get().IsItemPurchased(name);
		if (!purchased) 
			Get().StartItemPurchase(name);
		return purchased;
	}
	
	public static void SetNativeListener(long ptr)
	{
		Get().SetNativePurchaseListener(ptr);
	}
	
	// Singleton:
	
	public static PurchaseManager Get() 
	{
		if (inst==null) inst = new PurchaseManager();
		return inst;
	}
	
	private PurchaseManager() {}
	private static PurchaseManager inst = null;
	
	// ITEMS NAMES (ALWAYS KEEP THIS SYNCHED WITH NATIVE ENUM!) :
	
	public static final String SAVELOAD = "augmented_midi_controller_saveload";
	public static final String BLOW = "augmented_midi_controller_blow";
	public static final String SNAP = "augmented_midi_controller_snap";
	
	private Map<String,Integer> availableItemsMap;
	
	protected void InitAvailableItemsMap()
	{
		availableItemsMap = new HashMap<String,Integer>();
		availableItemsMap.put(SAVELOAD, NOT_PURCHASED);
		availableItemsMap.put(BLOW, NOT_PURCHASED);
		availableItemsMap.put(SNAP, NOT_PURCHASED);
	}
	
	public static String NativeIdToItemName(int id)
	{
		switch (id) {
		case 0: return SAVELOAD;
		case 1: return BLOW;
		case 2: return SNAP;
		default: return "";
		}
	}
	
	public static int ItemNameToNativeId(String name)
	{
		if (name.equals(SAVELOAD)) return 0;
		else if (name.equals(BLOW)) return 1;
		else if (name.equals(SNAP)) return 2;
		else return -1; 
	}
	
	protected static final String TAG = "SCDF Purchase Manager";
	
	private Activity theActivity = null;
	
	private IabHelper purchaseHelper=null;
	private final Object helperLock = new Object();
	
	public static final int NOT_PURCHASED = 0;
	public static final int PURCHASED = 1;
	public static final int UNKNOWN = 2; // try to remove
 	
	public static final String PREF_SUFFIX = "_purchase_state";
	
	public static String purchaseFailureMessage = "No additional information.";
	boolean inAppBillingSetupCompleted=false; // this becomes true when setup is over (no matter if successful or failed)
	boolean inAppBillingBroken=true;		  // this becomes false only when setup is over, if everything went OK.
	
	
	public boolean IsItemPurchased(String item) {
		Integer state = availableItemsMap.get(item);
		if (state==null) return false; // this check is enough only because we never store null values
		return state.intValue() == PURCHASED;
	}
	
	//public static boolean IsEffectsPurchased() { return effectsState==PURCHASED; }
	//public static boolean IsMidiPurchased() { return midiState==PURCHASED; }
	//public static boolean IsAdvancedPurchased() { return midiState==PURCHASED; }
		
	protected static void LOGI(String str) { Log.i(TAG,str); }
	private static void LOGD(String str) { Log.d(TAG,str); }
	private static void LOGE(String str) { Log.e(TAG,str); }
	
	public static final String myKey = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArOvOnVgGBpT+ADC6kVanT9O+IIhi6hxF/gt6GoNFGbhJ1nvIacSX4cRAyTp2sEEV7kPirsNiKye";
	
	public void StartSetup(Activity context, PurchaseListener listener)
	{
		LOGI("Start setup");
		InitAvailableItemsMap();
		
		// Set the Activity:
		theActivity = context;
		
		// Set Listener:
		thePurchaseListener = listener;
		
		// Setup and create the purchase helper:
				
		String myKey2 = "reES4YBTj0NkHwpMjnKViZsccIv5gYSFEkc8XsXmtlwogHfw/o8YSkE1zNw22a5xYW862NfheaCLHtKUPToCRqyzS5efKlvwM1RvWGUcV8m4aTG7UoI9K+88PmPT5hx12ly9Qx6YxSIF2GhACzKpWwGFhFA3XahDHBoctBF29cJC72FiRI25FBilqbN01AaGUYGBCNcTklNbwp8xVn3eGUKD+Sve7BR/TucnJKT4ye2omv50rNoIAukjzIfdd5wWrXGyTkF4LqwIDAQAB";
	    
		synchronized(helperLock) {
	       LOGI("calling helper constructor");
	       purchaseHelper = new IabHelper(context,myKey+myKey2);
	    }
	    purchaseHelper.enableDebugLogging(false);
	    
	    LOGI("loading states from preferences");
	    // While startSetup completes, we suppose that the purchase state has not changed
	    // since last time. At the end of startSetup the values will be updated anyway:
	    for (Map.Entry<String,Integer> entry : availableItemsMap.entrySet())
	    {
	    	entry.setValue( PrefManager.LoadInt(entry.getKey()+PREF_SUFFIX, NOT_PURCHASED));
	    }
	       
	    // Start The setup process:
	    inAppBillingSetupCompleted=false;
	    synchronized(helperLock) {
	    	LOGI("start helper SETUP");
	    	purchaseHelper.startSetup(this); // we indicated "this" as the "onIabSetupFinished" listener
	    }
	    // TODO: ri-verifica purchase a mano; salva nelle prefs i risultati dei vari passaggi del purchase
	    LOGI("start setup over.");
	}
	
	public void Dispose()
	{
		LOGI("Dispose helper!");
		
		inAppBillingSetupCompleted=false;
		thePurchaseListener=null;
		inAppBillingBroken=true;
		nativePurchaseListener = 0;
		theActivity=null;
		
		synchronized(helperLock) {
			purchaseHelper.dispose();
			purchaseHelper=null;
		}
		LOGI("Dispose completed");
	}
	
	
	
	public void StartItemPurchase(String item)
	{
		if (null==theActivity) 
			return;
		
		if (!inAppBillingSetupCompleted) {
			LOGE("Billing not ready, can't purchase "+item);
			Toast.makeText(theActivity,"In-app purchase not-ready yet, please retry later.",Toast.LENGTH_LONG).show();
			return;
		}
		if (inAppBillingBroken) {
			ItemPurchaseFailed(item,purchaseFailureMessage);
		}
		
		else {
			synchronized(helperLock) {
				if (null!=purchaseHelper) {
					LOGI("Launch purchase of "+item);
					purchaseHelper.launchPurchaseFlow(theActivity, item, 11111, this);
					// "this" is the purchase finished listener
				}
			}
		}
	}
	
	// Here the helper will fire the onIabPurchaseFinished callback:
	public boolean HandleActivityResult(int requestCode, int resultCode, Intent data)
	{
		boolean ret = false;
		synchronized(helperLock) {
			LOGD("handle activity result (if helper ok: "+purchaseHelper+")");
			if (purchaseHelper!=null)
				ret = purchaseHelper.handleActivityResult(requestCode, resultCode, data);
		}
		return ret; 
	}
	
	
	// V3 listeners:
   
	// This is called when startup setup has been completed:
	@Override
	public void onIabSetupFinished(IabResult result)
	{
		if (!result.isSuccess()) {
			purchaseFailureMessage = "Setup error - "+result.getMessage();
			LOGE("In-app billing setup failed: "+purchaseFailureMessage);
			inAppBillingBroken=true;
			inAppBillingSetupCompleted=true;
			return;
		}
		
		synchronized(helperLock) {
			LOGD("query for the inventory of the items (if helper ok: "+purchaseHelper+")");
			if (purchaseHelper!=null)  {
				List<String> availableItemsList = new ArrayList<String>(availableItemsMap.keySet());
				purchaseHelper.queryInventoryAsync(true,availableItemsList,this);
				// we pass "this" as the OnQueryInventoryFinished listener
			}
		}
	 }
	   
	 // this is called when asking for purchase inventory
	 // (for example when helper setup is completed):
	 @Override
	 public void onQueryInventoryFinished(IabResult result, Inventory inv)
	 {
		 LOGD("query inventory finished");
			
		 if (result.isFailure()) {
			 purchaseFailureMessage = "Inventory error - "+result.getMessage();
			 LOGE("Inventory query failed: "+purchaseFailureMessage);
			 inAppBillingBroken=true;
			 inAppBillingSetupCompleted=true;
			 return;
		 }
		 	
		 // Get the item's purchase state:
		 // NB, the purchase state is taken from the JSON string in the response data for a
		 // purchase order (see: http://developer.android.com/google/play/billing/billing_reference.html)
		 // Possible values are 0 (purchased), 1 (canceled), or 2 (refunded).		 
		 // When the item has never been purchased, inv.hasPurchase will return false, and calling
		 // inv.getPurchase(ITEMNAME) will return null;
		 
		 // save purchase state for the item and notify it to the listener:
		 for (Map.Entry<String,Integer> entry : availableItemsMap.entrySet())
		 {
			 if (inv.hasPurchase(entry.getKey())) {
				 
				 int state = inv.getPurchase(entry.getKey()).getPurchaseState();
				 if (state==0) { // 0: purchased
					 LOGD(entry.getKey()+" is purchased");
					 entry.setValue(PURCHASED);
					 ItemPurchaseStateChanged(entry.getKey(),entry.getValue());
				 } else { // purchased once but then cancelled or refunded
					 LOGD(entry.getKey()+" is refunded or cancelled");
					 entry.setValue(NOT_PURCHASED);
					 ItemPurchaseStateChanged(entry.getKey(),entry.getValue());
				 }
				 
			 } 
			 else { // no such item in the inventory => never purchased
				 LOGD(entry.getKey()+" has never been purchased");
				 entry.setValue(NOT_PURCHASED);
				 ItemPurchaseStateChanged(entry.getKey(),entry.getValue());
			}
		 } // end of loop through available items
		 
		 inAppBillingBroken=false;
		 inAppBillingSetupCompleted=true;
	 }
	 
	 
	 // this is called when the purchase flow has been completed:
	 @Override
	 public void onIabPurchaseFinished(IabResult result, Purchase purchaseInfo)
	 {
		 LOGD("Purchase procedure for finished");
		 if (result.isFailure()) {
			 LOGE("purchase has failed!");
			 if (result.getResponse()!=IabHelper.IABHELPER_USER_CANCELLED) { 
				 // avoid showing error for intentional cancel	
				 purchaseFailureMessage = "Purchase error - "+result.getMessage();
				 ItemPurchaseFailed("failed_item",purchaseFailureMessage);
			 }
			 return;
		 }
		 String sku = purchaseInfo.getSku();
		 if (availableItemsMap.containsKey(sku) ) {
			 LOGD(sku+" purchased correctly! :) ");
			 availableItemsMap.put(sku,PURCHASED); // since the sku is contained in the map, it's value will be updated
			 ItemPurchaseStateChanged(sku,PURCHASED);
		 }
	 }
	 
	 
	 private PurchaseListener thePurchaseListener = null;
	 private long nativePurchaseListener = 0;
	 private void SetNativePurchaseListener(long ptr) { nativePurchaseListener=ptr;}
	 private native void CallNativePurchaseChanged(long listenerPtr,int itemNativeId, int state);
	 private native void CallNativePurchaseFailed(long listenerPtr,int itemNativeId,String msg);
	 
	 private void ItemPurchaseStateChanged(String itemName, int state)
	 {
		 // sposta qui pure l'update della value per la key itemName nella map
		 LOGD("Item: "+itemName+", purchase state changed. Now it's: "+state);
		 PrefManager.SaveInt(itemName+PREF_SUFFIX,state);		 
		 if (thePurchaseListener!=null)
			 thePurchaseListener.OnItemPurchaseStateChange(itemName,state);
		 if (nativePurchaseListener!=0)
			 CallNativePurchaseChanged(nativePurchaseListener,ItemNameToNativeId(itemName),state);
	 }
	 
	 private void ItemPurchaseFailed(String itemName,String message) {
		 if (thePurchaseListener!=null)
			 thePurchaseListener.OnPurchaseFailed(itemName,message);
		 if (nativePurchaseListener!=0)
			 CallNativePurchaseFailed(nativePurchaseListener,ItemNameToNativeId(itemName),message);

	 }
	 
	 	 
	 
}
