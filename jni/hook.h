
#include <stdio.h>
#include <android/log.h>
#include <unistd.h>
#include <jni.h>
#include <string>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "kkkHook", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "kkkHook", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "kkkHook", __VA_ARGS__))

#ifdef __cplusplus
extern "C" {
#endif

    void _init(void);
    
#ifdef __cplusplus
}
#endif

class hookNative {
public:
	static void __attribute__((constructor)) init_kkk()
	{
		LOGI("kkk%s", "native hook is start");
	};
	static uint32_t get_module_base(pid_t pid, const char *module_path);
	static const char* get_process_name(pid_t pid);
	static jobject getApplication(JNIEnv *env);
	static std::string getPackageName(JNIEnv *env);
	static std::string jstring2str(JNIEnv* env, jstring jstr);
};