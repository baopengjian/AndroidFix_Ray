#include <jni.h>
#include <string>
#include "art_method.h"
#include "dalvik.h"

typedef Object *(*FindObject)(void *thread, jobject jobject1);

typedef void *(*FindThread)();

FindObject findObject;
FindThread findThread;

extern "C" JNIEXPORT jstring
JNICALL
Java_com_cn_ray_androidfix_1ray_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//5.0
extern "C"
JNIEXPORT void JNICALL
Java_com_cn_ray_androidfix_1ray_DexManager_replaceDavik(JNIEnv *env, jobject instance, jint sdk,
                                                        jobject wrongMethod, jobject rightMethod) {
//    做  跟什么有关   虚拟机    java虚拟机 Method
//找到虚拟机对应的Method 结构体

    Method *wrong = (Method *) env->FromReflectedMethod(wrongMethod);
    Method *right = (Method *) env->FromReflectedMethod(rightMethod);
    //下一步  把right 对应Object   第一个成员变量ClassObject   status


//    ClassObject
    void *dvm_hand = dlopen("libdvm.so", RTLD_NOW);
    //    sdk  10    以前是这样   10会发生变化
    findObject = (FindObject) dlsym(dvm_hand,
                                    sdk > 10 ? "_Z20dvmDecodeIndirectRefP6ThreadP8_jobject"
                                             : "dvmDecodeIndirectRef");

    findThread = (FindThread) dlsym(dvm_hand, sdk > 10 ? "_Z13dvmThreadSelfv" : "dvmThreadSelf");

    // method   所声明的Class
    jclass methodClaz = env->FindClass("java/lang/reflect/Method");
    jmethodID rightMethodId = env->GetMethodID(methodClaz, "getDeclaringClass",
                                               "()Ljava/lang/Class;");

    //dalvik  odex   机器码
//    firstFiled->status=CLASS_INITIALIZED
//    art不需要    dalvik适配

    jobject ndkObject = env->CallObjectMethod(rightMethod, rightMethodId);
    //    kclass
//    davik   ------>ClassObject
    ClassObject *firstFiled = (ClassObject *) findObject(findThread(), ndkObject);
    firstFiled->status = CLASS_INITIALIZED;
    wrong->accessFlags |= ACC_PUBLIC;
    //对于具体已经实现了的虚方法来说，这个是该方法在类虚函数表（vtable）中的偏移；对于未实现的纯接口方法来说，
// 这个是该方法在对应的接口表（假设这个方法定义在类继承的第n+1个接口中，则表示iftable[n]->methodIndexArray）中的偏移；
    wrong->methodIndex = right->methodIndex;
    //    这个变量记录了一些预先计算好的信息，从而不需要在调用的时候再通过方法的参数和返回值实时计算了，方便了JNI的调用，提高了调用的速度。
// 如果第一位为1（即0x80000000），则Dalvik虚拟机会忽略后面的所有信息，强制在调用时实时计算；
    wrong->jniArgInfo = right->jniArgInfo;
    wrong->registersSize = right->registersSize;
    wrong->outsSize = right->outsSize;
    //    方法参数 原型
    wrong->prototype = right->prototype;
    wrong->insns = right->insns;
    //    如果这个方法是一个Dalvik虚拟机自带的Native函数（Internal Native）的话，
// 则这里存放了指向JNI实际函数机器码的首地址。如果这个方法是一个普通的Native函数的话，
// 则这里将指向一个中间的跳转JNI桥（Bridge）代码；

    wrong->nativeFunc = right->nativeFunc;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cn_ray_androidfix_1ray_DexManager_replace(JNIEnv *env, jobject instance, jint sdk,
                                                   jobject wrongMethod, jobject rightMethod) {

    art::mirror::ArtMethod *wrong = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(
            wrongMethod));
    art::mirror::ArtMethod *right = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(
            rightMethod));

    wrong->declaring_class_ = right->declaring_class_;
    wrong->dex_cache_resolved_methods_ = right->dex_cache_resolved_methods_;
    wrong->access_flags_ = right->access_flags_;
    wrong->dex_cache_resolved_types_ = right->dex_cache_resolved_types_;
    wrong->dex_code_item_offset_ = right->dex_code_item_offset_;
    wrong->dex_method_index_ = right->dex_method_index_;
    wrong->method_index_ = right->method_index_;
}