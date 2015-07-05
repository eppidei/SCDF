/*
 * InAppAndroid.cpp
 *
 *  Created on: 6 May 2015
 *      Author: athos
 */

#include "InAppPurchase.h"
#include "JniGlue.h"

bool CheckIsInAppPurchased(PurchaseProductIndex index)
{
#ifdef _DEBUG
	return true;
#else
	JNIEnv* env = Jni::Env();
	jclass pMan = Jni::FindClass("it/scdf/controller/PurchaseManager");
	jmethodID method = Jni::FindStaticMethod("IsItemPurchasedShowInvite","(I)Z",pMan,env);
	jboolean purchased = env->CallStaticBooleanMethod(pMan,method,index);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	if (exc) return false;
	return purchased;
#endif
}

bool CheckIsInAppPurchasedNoPrompt(PurchaseProductIndex index)
{
#ifdef _DEBUG
	return true;
#else
	JNIEnv* env = Jni::Env();
	jclass pMan = Jni::FindClass("it/scdf/controller/PurchaseManager");
	jmethodID method = Jni::FindStaticMethod("IsItemPurchased","(I)Z",pMan,env);
	jboolean purchased = env->CallStaticBooleanMethod(pMan,method,index);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	if (exc) return false;
	return purchased;
#endif
}
