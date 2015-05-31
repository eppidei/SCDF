#include "JniGlue.h"
#include <pthread.h>

/* Java type signature reference:
Z	boolean
B	byte
C	char
S	short
I	int
J	long
F	float
D	double
Lfully-qualified-class;	fully-qualified-class
[ type	type[]
*/

// this function is called when library is loaded:

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("Caching java virtual machine reference");
    Jni::InitThreadDetachKey();
	Jni::SetJvm(jvm);

	bool somethingWrong = false;
	if ( ! Jni::CacheClass("it/scdf/framework/UsbHandler")) somethingWrong = true;
	if ( ! Jni::CacheClass("it/scdf/framework/ForegroundActivity")) somethingWrong = true;

	if ( ! Jni::CacheClass("it/scdf/controller/PurchaseManager")) somethingWrong = true;
	// TODO: move this to scdf controller app code

	// ADD HERE ALL CLASSES YOU WANT TO CACHE

	if (somethingWrong) {
		LOGE("Java classes caching failed!");
		#ifdef _DEBUG
		int* crashme;
		LOGE("Class loading failed... Harakiri!!! %d",*crashme);
		#endif
	}

    return JNI_VERSION_1_6;
}



JavaVM* Jni::jvm = NULL;
pthread_key_t Jni::threadDetachKey;
std::map<std::string,jclass> Jni::cachedClasses;

bool Jni::CacheClass(std::string classname)
{
	if( GetCachedClass(classname) ) return true;
	JNIEnv* env = Env();
	jclass cl = (jclass) env->FindClass(classname.c_str());
	if (!cl) {
		// If this happens you're basically doomed.
		// And this may happen if you didn't cache the class in JNI_OnLoad()
		LOGE("Error in env->FindClass for %s",classname.c_str());
		return false;
	}
	jclass classGlobalRef = (jclass)env->NewGlobalRef(cl);
	if (!classGlobalRef) {
		// If this happens, you are again doomed, and very unlucky indeed
		LOGE("Error in env->NewGlobalRef for %s",classname.c_str());
		return false;
	}
	cachedClasses[classname] = classGlobalRef;
	return true;
}

jclass Jni::GetCachedClass(std::string classname)
{
	if (0==cachedClasses.count(classname)) return 0;
	return cachedClasses[classname];
}

jclass Jni::FindClass(std::string classname)
{
	jclass cl = GetCachedClass(classname);
	if (cl!=0) return cl; // this is a Global ref (all cached classes are global refs!)
	if (!CacheClass(classname)) return 0;
	return GetCachedClass(classname);
}

jmethodID Jni::FindMethod(std::string name, std::string signature, jclass javaClass, JNIEnv* env)
{
	if (env==0) env = Env();
	jmethodID id = env->GetMethodID(javaClass, name.c_str(), signature.c_str());
	if (CheckForException("GetMethodID (in Jni::FindMethod)",env)) return 0;
	return id;
}


jmethodID Jni::FindStaticMethod(std::string name, std::string signature, jclass javaClass, JNIEnv* env)
{
	if (env==0) env = Env();
	jmethodID id = env->GetStaticMethodID(javaClass, name.c_str(), signature.c_str());
	if (CheckForException("GetStaticMethodID (in JNI::FindStaticMethod)",env)) return 0;
	return id;
}



void Jni::InitThreadDetachKey()
{
	pthread_key_create(&threadDetachKey,Jni::DetachCurrentThread);
}

// This callback is automatically called when a thread exits:
// Only if there is a pthread_key associated with it and if
// there is a non-null value associated with the key:

void Jni::DetachCurrentThread(void* venv)
{
	//LOGD(" DETACHING CURRENT THREAD FROM JNI!");
	Jvm()->DetachCurrentThread();
}

JNIEnv* Jni::Env()
{
	if (0==jvm) return 0;

	// Try getting the Env for the current thread, and contextually
	// check whether the thread is attached, and attach if not.
	// Since we are setting a "specific" for each thread we attach,
	// we may be tempted to check for thread detach state using:
	// if ((env = pthread_getspecific (threadDetachKey)) == 0)
	// but with this check also the main java thread would show as
	// detached (we never manually attached called setspecific for it).

	JNIEnv* env;

	if (jvm->GetEnv((void **)&env,JNI_VERSION_1_6)==JNI_EDETACHED)
	{
		jvm->AttachCurrentThread(&env,0);
		pthread_setspecific(threadDetachKey, env);
		// "env" will be passed to the destroy callback
		// NB: we are not using the env value associated with the key
		// in the destroy callback, but we need to set the specific
		// anyway, so that conditions are met for the call of the callback
		// (the key must have a non-null value associated with it).
	}
	return env;
}

bool Jni::CheckForException(std::string info, JNIEnv* env)
{
	if (JNI_TRUE==env->ExceptionCheck()) {
		LOGE(" JNI exception calling: %s",info.c_str());
	    env->ExceptionDescribe();
	    env->ExceptionClear();
	    return true;
	}
	return false;
}




