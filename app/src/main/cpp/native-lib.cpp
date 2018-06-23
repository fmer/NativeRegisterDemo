#include <jni.h>
#include "mDebug.h"
#include <string>
#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

static jstring j_hello(JNIEnv* env, jobject thiz )
{
         const char* hello  = "Hello from C++";
         return env->NewStringUTF(hello);
}
static jstring j_hello2(JNIEnv* env, jobject thiz )
{
         const char* hello  = "test-----------------------------2";
         return env->NewStringUTF(hello);
}

/**
 * 绑定对应的方法映射表
 */
static const JNINativeMethod jniMethods[] = {
        {"stringFromJNI", "()Ljava/lang/String;", (void*)j_hello},
        {"stringFromJNI2", "()Ljava/lang/String;", (void*)j_hello2},
};

static int  registerMethods(JNIEnv * env, const char* className ,const JNINativeMethod* gMethods, int numMethods) {
         jclass clazz;
         clazz =env->FindClass( className);
         if (clazz == NULL) {
                  return JNI_FALSE;
         }
         if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
                  return JNI_FALSE;
         }
         return JNI_TRUE;
}

/**
 * 注册所有方法
 */
static int registerAllMethods(JNIEnv* env) {
         const char* kClassName = "com/blur/blurbyjnidemo/NativeLib";//指定要注册的类
         return registerMethods(env, kClassName,jniMethods,  NELEM(jniMethods));
}

/***
 *jni_onload加载时注册映射表中的方法
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
         JNIEnv* env = NULL;
         if (vm->GetEnv( (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
                  return -1;
         }
         mInfo("JNI_OnLoad");
         if (!registerAllMethods(env)) {//注册所有方法
                  return -1;
         }
         return JNI_VERSION_1_4;
}