#ifndef SCDF_ANDROID_JNI_GLUE_h
#define SCDF_ANDROID_JNI_GLUE_h

#include <jni.h>
#include "Logging.h"
#include <string>
#include <map>

// You can get class members' signatures calling:
// javap -classpath bin/classes -s com.ntrack.the.class.you.want
// from android project's directory.
// follow JNI tips at: http://developer.android.com/training/articles/perf-jni.html


class JavaMethodInfo {
	std::string name;
	std::string signature;
	jmethodID 	id;
};

class Jni {

public:

	static void SetJvm(JavaVM* vm) { jvm = vm; }
	static JavaVM* Jvm() { return jvm; }

	static JNIEnv* Env();
	// caching the jni env is NOT thread safe (each thread has its own)

	friend JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved);
	// this function will populate the classes cache

	static jclass FindClass(std::string classname);
	static jmethodID FindMethod(std::string name, std::string signature, jclass javaClass, JNIEnv* env = 0);
	static jmethodID FindStaticMethod(std::string name, std::string signature, jclass javaClass, JNIEnv* env = 0);

	static bool CacheClass(std::string classname);

	static void InitThreadDetachKey();
	static void DetachCurrentThread(void* env); // called on a thread's death
	static bool CheckForException(std::string methodInfo, JNIEnv* env); // returns true on exception

private:

	static JavaVM* jvm;
	static pthread_key_t threadDetachKey; // needed to append the detach callback

	// FindClass may fail if called from user-created c++ threads. So we cache them:
	// http://developer.android.com/training/articles/perf-jni.html#faq_FindClass
	static std::map<std::string,jclass> cachedClasses;

	static jclass GetCachedClass(std::string classname);
};


class JavaClass {

public:

	JavaClass() : theJavaObject(0) {}

protected:

	jobject theJavaObject;

    bool SetupJavaObject(jobject inObject)
    {
    	JNIEnv* env = Jni::Env();
    	// get a global reference (to avoid problems between different jni calls):
    	if (0!=theJavaObject)
    		env->DeleteGlobalRef(theJavaObject);
    	theJavaObject = env->NewGlobalRef(inObject);
    	return theJavaObject!=0;
    }

	~JavaClass()
	{
		// release global references
		JNIEnv* env = Jni::Env();
		if (0!=theJavaObject)
			env->DeleteGlobalRef(theJavaObject);
	}



};

#endif
