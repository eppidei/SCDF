#include <string>
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "RTP Test"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


extern "C" JNIEXPORT jstring JNICALL Java_com_scdf_rtptest_MainActivity_TestA(JNIEnv* e, jobject thiz)
{
	std::string res;
	char str[2000];
	str[0]='A';
	str[1]='\0';
	return e->NewStringUTF(str);
}


extern "C" JNIEXPORT jstring JNICALL Java_com_scdf_rtptest_MainActivity_TestB(JNIEnv* e, jobject thiz)
{
	char str[2000];
	str[0]='B';
	str[1]='\0';
	return e->NewStringUTF(str);
}


extern "C" JNIEXPORT jstring JNICALL Java_com_scdf_rtptest_MainActivity_TestC(JNIEnv* e, jobject thiz)
{
	char str[2000];
	str[0]='C';
	str[1]='\0';
	return e->NewStringUTF(str);
}
