#include <jni.h>
#include <string>

#define _MAKE_JNI_FUNCTION(r, n, p) extern "C" JNIEXPORT r JNICALL Java_ ## p ## _ ## n
#define JNI(r, n) _MAKE_JNI_FUNCTION(r, n, com_stdlib_industrialice_stdlib_MainActivity)

extern int main();

JNI(jstring, stringFromJNI)(JNIEnv *env, jobject)
{
    std::string hello = "Hello from C++";
    main();
    return env->NewStringUTF(hello.c_str());
}
