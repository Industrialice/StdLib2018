#include <jni.h>
#include <string>

#define _MAKE_JNI_FUNCTION(r, n, p) extern "C" JNIEXPORT r JNICALL Java_ ## p ## _ ## n
#define JNI(r, n) _MAKE_JNI_FUNCTION(r, n, com_stdlib_industrialice_stdlib_MainActivity)

extern int main(int argc, const char **argv);

JNI(jstring, stringFromJNI)(JNIEnv *env, jobject, jstring appPath)
{
    std::string hello = "Hello from C++";
    const char *cStr = env->GetStringUTFChars(appPath, NULL);
    const char *args[] =
            {
                    "AndroidApp",
                    cStr
            };
    main(sizeof(args) / sizeof(void *), args);
    env->ReleaseStringUTFChars(appPath, cStr);
    return env->NewStringUTF(hello.c_str());
}
