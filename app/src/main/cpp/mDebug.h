#ifndef __MDEBUG_H__
#define __MDEBUG_H__
#include <jni.h>
#include <android/log.h>
#ifndef BASETYPES
#define BASETYPES

//typedef _Null_terminated_ char *PSZ;
#endif  /* !BASETYPES */
#ifndef CAMERA_LOG_TAG
#define CAMERA_LOG_TAG	"debug"
#define mDebug(format, ...) __android_log_print(ANDROID_LOG_ERROR,CAMERA_LOG_TAG, format" [File:%s, Line:%d, Function:%s]",##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__)
#define mInfo(format, ...)  __android_log_print(ANDROID_LOG_INFO,CAMERA_LOG_TAG, format" [File:%s, Line:%d, Function:%s]",##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__)
#define mMsg(...)  __android_log_print(ANDROID_LOG_INFO,CAMERA_LOG_TAG, __VA_ARGS__)
#endif

#endif
