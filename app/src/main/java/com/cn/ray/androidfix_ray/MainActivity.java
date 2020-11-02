package com.cn.ray.androidfix_ray;

import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    //固定 不会发生变化   方法----字节手机型号                 Android 版本 不同 而不同
    public void test(View view) {
        Caclutor caclutor = new Caclutor();
        caclutor.test(this);
//        new   反射    classloader.load()
    }

    public void fix(View view) {
        DexManager dexManager = new DexManager();
        dexManager.setContext(this);
        dexManager.load(new File(Environment.getExternalStorageDirectory(), "out.dex"));
    }
}
