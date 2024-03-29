/*
 * Logging.h
 *
 *  Created on: Jun 22, 2014
 *      Author: athos
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#ifdef  _DEBUG

#ifdef ANDROID
#include <android/log.h>
#define  LOG_TAG    "SCDF"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGD(...) printf(__VA_ARGS__);
#define LOGE(...) printf(__VA_ARGS__);
#define LOGI(...) printf(__VA_ARGS__);
//#define LOGD(...)
//#define LOGE(...)
//#define LOGI(...)
#endif

#else

#if defined(ANDROID) && !defined(_RTP_MIDI_PROJECT)
#include <android/log.h>
#define  LOG_TAG    "SCDF"
#define  LOGD(...)  //__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGD(...)
#define LOGE(...)
#define LOGI(...)
#endif

#endif

#endif /* LOGGING_H_ */
