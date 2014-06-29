/*
 * Logging.h
 *
 *  Created on: Jun 22, 2014
 *      Author: athos
 */

#ifndef LOGGING_H_
#define LOGGING_H_

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
#endif

#endif /* LOGGING_H_ */
