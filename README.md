### JNI Native动态注册演练 ###

#### 前言：####
*前几天分享了一篇关于TS流解析的文章，有朋友在问，你怎么不使用动态注册呀，什么是JN动态注册呢？今天给大家介绍使用一下。*
#### 一、JNI Native注册介绍 ####
*jni native注册主要有2种，静态注册和动态注册，其中静态注册是我们常用的，因为部分项目用到jni的接口很少，通过静态注册就能很方便快速的实现，不过当接口多起来时就会略显麻烦，并且静态注册的包名关联，很容易导致错误，排版也不好看，而动态注册就很好的解决了这一问题。*

1. 静态注册：
	* 特点：实现快速，但是函数名也非常长，不适合管理
	* 它的编译形式是根据函数名来遍历查找java和jni函数之间的关联，然后静态调用

			extern "C" JNIEXPORT jstring JNICALL
			Java_com_blur_blurbyjnidemo_MainActivity_stringFromJNI(JNIEnv* env,jobject /* this */) {
			    std::string hello = "Hello from C++";
			    return env->NewStringUTF(hello.c_str());
			}
2. 动态注册：
	* 特点：每个方法一一映射，简单明了，不容易出错，便于管理
	* 它通过在jvm中注册jni函数映射表，再根据函数映射表去调用对应名称和参数的函数，

				/*
				 * used in RegisterNatives to describe native method name, signature,
				 * and function pointer.from jni.h
				 */
				typedef struct {
					const char* name;//java方法名称
					const char* signature;//jni对应的函数参数和返回值的描述值,具体参考第三节
					void* fnPtr;//fnPtr是函数指针，指向C函数
				} JNINativeMethod;

#### 二、动态注册的具体事例 ####
1. 动态注册的几个步骤：
	* 绑定对应的方法映射表
	* 在JNI_OnLoad时注册映射表中的方法
	* 下面是demo代码

	**native-lib.cpp：**

			#include <jni.h>
			#include "mDebug.h"
			#include <string>
			#ifndef NELEM
			#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
			#endif
			//c测试方法
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
			 * 1. 绑定对应的方法映射表
			 */
			static const JNINativeMethod jniMethods[] = {
			        {"stringFromJNI", "()Ljava/lang/String;", (void*)j_hello},
			        {"stringFromJNI2", "()Ljava/lang/String;", (void*)j_hello2},
			};
			/**注册方法*/
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
			 * 2. jni_onload加载时注册映射表中的方法
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

	**mDebug.h：**

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
	**NativeLib.java：**

			package com.blur.blurbyjnidemo;
			public class NativeLib {
			         // Used to load the 'native-lib' library on application startup.
			         static {
			                  System.loadLibrary("native-lib");
			         }

			         /**
			          * A native method that is implemented by the 'native-lib' native library,
			          * which is packaged with this application.
			          */
			         public  native String stringFromJNI();
			         public static native String stringFromJNI2();
			}
#### 三、jni对应参数映射表 ####
上面动态注册结构第二项参数，需要填入参数和方法返回的映射值：

**这个字符串的结构是一个括号后面再接字符串：**

**1. "()"中的描述的是函数的传入参数描述**

**2. 括号后面接的是返回值描述**
**比如："()V"：表示 void function();  (JF)Z"：表示 boolean function(long l,float f);等**
**基本类型类型对照表**

		V      void            void
		Z       jboolean     boolean
		I        jint              int
		J       jlong            long
		D      jdouble       double
		F      jfloat            float
		B      jbyte            byte
		C      jchar           char
		S      jshort          short

		数组在前面加"[",如：
			[I       jintArray      int[]

**非基本类型，比如class类型的，以"L"开头，通过"/"隔开包名与类名，以";"结尾，比如：**

		Ljava/lang/String; String jstring

#### 以上就是JNI动态注册的正常使用步骤，当然在项目中还可以更具体的封装，掌握动态注册是进行大型项目开发的必修课~ ####
