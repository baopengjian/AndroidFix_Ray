package com.cn.ray.androidfix_ray;

import android.content.Context;
import android.os.Build;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.Enumeration;

import dalvik.system.DexFile;

public class DexManager {

    private Context context;

    static {
        System.loadLibrary("native-lib");
    }

    public void setContext(Context context) {
        this.context = context;
    }

    public void load(File file) {
        try {
            DexFile dexFile = DexFile.loadDex(file.getAbsolutePath(), new File(context.getCacheDir(), "opt").getAbsolutePath(), Context.MODE_PRIVATE);
            Enumeration<String> entry = dexFile.entries();
            while (entry.hasMoreElements()) {
                //全类名
                String className = entry.nextElement();
                Class realClazz = dexFile.loadClass(className, context.getClassLoader());
                if (realClazz != null) {
                    fixClass(realClazz);
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void fixClass(Class realClazz) {
        //加载方法 Method
        Method[] methods = realClazz.getMethods();
        for (Method rightMethod : methods) {
            Replace replace = rightMethod.getAnnotation(Replace.class);
            if (replace == null) {
                continue;
            }
            String clazzName = replace.clazz();
            String methodName = replace.method();

            try {
                Class wrongClazz = Class.forName(clazzName);
                Method wrongMethod = wrongClazz.getDeclaredMethod(methodName, rightMethod.getParameterTypes());
                if (Build.VERSION.SDK_INT <= 18) {
                    replaceDavik(Build.VERSION.SDK_INT,wrongMethod, rightMethod);
                } else {
                    replace(Build.VERSION.SDK_INT,wrongMethod, rightMethod);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

    }

    public native  void replaceDavik(int sdk,Method wrongMethod, Method rightMethod);


    public native  void replace(int sdk,Method wrongMethod, Method rightMethod);

}
