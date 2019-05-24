#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <android/log.h>

#include "hook.h"
#include <jni.h>

uint32_t hookNative::get_module_base(pid_t pid, const char *module_path) 
{
    FILE *fp = NULL;
    char *pch = NULL;
    char filename[32];
    char line[512];
    uint32_t addr = 0;

    if ( pid < 0 ) 
        snprintf(filename, sizeof(filename), "/proc/self/maps");
    else 
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);

    if ( (fp = fopen(filename, "r")) == NULL ) 
    {
        LOGE("open %s failed!", filename);
        return 0;
    }

    while ( fgets(line, sizeof(line), fp) ) 
    {
        if ( strstr(line, module_path) ) 
        {
            pch = strtok(line, "-");
            addr = strtoul(pch, NULL, 16);
            break;
        }
    }

    fclose(fp);

    return addr;
}

std::string hookNative::jstring2str(JNIEnv* env, jstring jstr)
{   
    char*   rtn   =   NULL;   
    jclass   clsstring   =   env->FindClass("java/lang/String");   
    jstring   strencode   =   env->NewStringUTF("GB2312");   
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");   
    jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);   
    jsize   alen   =   env->GetArrayLength(barr);   
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);   
    if(alen   >   0)   
    {   
        rtn   =   (char*)malloc(alen+1);         
        memcpy(rtn,ba,alen);   
        rtn[alen]=0;   
    }   
    env->ReleaseByteArrayElements(barr,ba,0);   
    std::string stemp(rtn);
    free(rtn);
    return   stemp;   
}



/**
 * 获得context
 * @param env
 * @return
 */
jobject hookNative::getApplication(JNIEnv *env) {
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        }
        env->DeleteLocalRef(activity_thread_clz);
    }
    return application;
}

/**
 * 获得包名
 * @param env
 * @return
 */
std::string hookNative::getPackageName(JNIEnv *env) {
    jobject context = getApplication(env);
    // 获得Context类
    jclass cls = env->GetObjectClass(context);
    // 得到getPackageManager方法的ID
    jmethodID mid = env->GetMethodID(cls, "getPackageManager",
                                     "()Landroid/content/pm/PackageManager;");
    // 获得应用包的管理器
    jobject pm = env->CallObjectMethod(context, mid);
    // 得到getPackageName方法的ID
    mid = env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
    // 获得当前应用包名
    jstring packageName = (jstring) env->CallObjectMethod(context, mid);
    const char *c_pack_name = env->GetStringUTFChars(packageName, NULL);
    // 比较包名,若不一致，直接return包名
//     if (strcmp(c_pack_name, PACKAGE_NAME) == 0) {
// //        verPackage = 1;
//     }

    std::string appName = jstring2str(env, (env)->NewStringUTF(c_pack_name));
    return appName;
}



