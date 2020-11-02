package com.cn.ray.androidfix_ray.web;

import android.content.Context;
import android.widget.Toast;

import com.cn.ray.androidfix_ray.Replace;

public class Caclutor {
    
    @Replace(clazz = "com.cn.ray.androidfix_ray.Caclutor", method = "test")
    public void test(Context context) {
        Toast.makeText(context, "修复了", Toast.LENGTH_SHORT).show();
    }
}
