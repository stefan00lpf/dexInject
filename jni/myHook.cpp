#include <stdio.h>

#include <android/log.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>

#include <jni.h>

#include "hook.h"
#include "hookzz.h"
#include <iostream>

typedef JNIEnv* (*JNIFUN)(void); 

JNIEnv* (*getJNIEnv)();

int invoke_dex_method(const char* dexPath, const char* dexOptDir, const char* className, const char* methodName, int argc, char *argv[]) {
    //获取JNIEnv
    void* handle = dlopen("/system/lib/libandroid_runtime.so", RTLD_NOW);
    getJNIEnv = (JNIFUN)dlsym(handle, "_ZN7android14AndroidRuntime9getJNIEnvEv");
    JNIEnv* env = getJNIEnv();

    //调用ClassLoader中的getSystemClassLoader方法获取当前进程的ClassLoader
    jclass classloaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getsysloaderMethod = env->GetStaticMethodID(classloaderClass, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject loader = env->CallStaticObjectMethod(classloaderClass, getsysloaderMethod);

    //以进程现有的ClassLoader、要注入的dex路径为参数构造注入后的DexClassLoader
    jstring dexpath = env->NewStringUTF(dexPath);
    jstring dex_odex_path = env->NewStringUTF(dexOptDir);
    jclass dexLoaderClass = env->FindClass("dalvik/system/DexClassLoader");
    jmethodID initDexLoaderMethod = env->GetMethodID(dexLoaderClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    jobject dexLoader = env->NewObject(dexLoaderClass, initDexLoaderMethod,dexpath,dex_odex_path,NULL,loader);

    //获取新出炉的DexClassLoader中findClass方法加载dex中要执行代码所在类
    jmethodID findclassMethod = env->GetMethodID(dexLoaderClass,"findClass","(Ljava/lang/String;)Ljava/lang/Class;");
    jstring javaClassName = env->NewStringUTF(className);
    jclass javaClientClass = (jclass)env->CallObjectMethod(dexLoader,findclassMethod,javaClassName);

    //获取注入dex中要执行的方法
    jmethodID start_inject_method = env->GetStaticMethodID(javaClientClass, methodName, "()V");
    //执行之注意目标方法必须是静态公有的
    env->CallStaticVoidMethod(javaClientClass,start_inject_method);

    return 0;
}

void doJavaInject()
{
    int ret = invoke_dex_method("/data/injects/DexInject.apk","/data/data/stefan.myhookdemo/cache","kkk/dexinject/HookTool","dexInject",0,NULL);
    LOGI("kkk inject java:%x", ret);
    return;
}

// void doNativeInject()
// {
//     void* handler = new char;
//     char* err = new char;

//     handler = dlopen("/data/data/com.jzL.cb.guopan/lib/libmain.so", RTLD_NOW);  
//     err = (char*)dlerror();
//     if (handler == NULL)
//         LOGD("dlopen libmain.so error is:%s\n",err);

//     handler = dlopen("/data/data/com.zyxd.mycm.guopan/lib/libunity.so", RTLD_NOW);
//     err = (char*)dlerror();
//     if (handler == NULL)
//         LOGD("dlopen libunity.so error is:%s\n",err);


//     uint32_t module_base_game = hookNative::get_module_base(getpid(), "libunity.so");
//     LOGI("the libunity.so is %x", module_base_game);

//     if(module_base_game)
//     {
        
//     }
// }

__attribute__((constructor)) void entry()
{

    pid_t pid = getpid();
    LOGI("kkk hook is start:%x", pid);

    //注入Dex
    doJavaInject();

    //注入native
    // doNativeInject();
}
