/*
 * OsUtilitiesAndroid.cpp
 *  Created on: 31 Jan 2015
 *      Author: athos
 */


#include "OsUtilities.h"
#include "JniGlue.h"

std::string scdf::GetAppDataDirectory()
{
	JNIEnv* env = Jni::Env();
	jclass foregroundActivity = Jni::FindClass("it/scdf/framework/ForegroundActivity");
	jmethodID method = Jni::FindStaticMethod("GetCacheDir","()Ljava/lang/String;",foregroundActivity,env);
	jstring jpath = (jstring)env->CallStaticObjectMethod(foregroundActivity,method);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	if (exc)
		return "";
	const char* path = env->GetStringUTFChars(jpath,0);
	std::string pathString(path);
	env->ReleaseStringUTFChars(jpath,path);
	return pathString;
}

std::string scdf::GetUserDataDirectory()
{
	JNIEnv* env = Jni::Env();
	jclass foregroundActivity = Jni::FindClass("it/scdf/framework/ForegroundActivity");
	jmethodID method = Jni::FindStaticMethod("GetFilesDir","()Ljava/lang/String;",foregroundActivity,env);
	jstring jpath = (jstring)env->CallStaticObjectMethod(foregroundActivity,method);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	if (exc)
		return "";
	const char* path = env->GetStringUTFChars(jpath,0);
	std::string pathString(path);
	env->ReleaseStringUTFChars(jpath,path);
	return pathString;
}


std::string scdf::GetDirectoryForFile(std::string file, std::string ext)
{
	return file;
}


void showTestAlert(std::string msg)
{
	//LOGD(mgs.c_str());
}
