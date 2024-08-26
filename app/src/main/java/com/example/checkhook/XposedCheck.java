package com.example.checkhook;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;

import com.example.monitordevice.CheckHookActivity;

import java.util.ArrayList;
import java.util.List;

public class XposedCheck {
    protected CheckHookActivity checkHookActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("checkhook");
    }

    public XposedCheck(CheckHookActivity checkHookActivity) {
        this.checkHookActivity = checkHookActivity;
    }

    private boolean checkMemory() {
        ArrayList<ClassLoader> choose = getClassLoader.choose(ClassLoader.class, true); // true的话，会获取ClassLoader所有的实例，包含其子类
//        Log.i(TAG, "Object Number: " + choose.size());
        for (ClassLoader classLoader : choose) {
            Class<?> clazz = null;
            try {
                clazz = Class.forName("de.robv.android.xposed.XC_MethodHook", false, classLoader);
                if (clazz != null) {
                    Log.i(TAG, "ClassLoader: " + classLoader + "      class: " + clazz);
                    return true;
                }
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
        }
        return false;
    }

    private boolean checkPmBundle(Context context) {
        PackageManager packageManager = context.getPackageManager();
        @SuppressLint("QueryPermissionsNeeded")
        List<PackageInfo> installedPackages =
                packageManager.getInstalledPackages(PackageManager.GET_META_DATA);
        for (int i = 0; i < installedPackages.size(); i++) {
            Bundle metaData = installedPackages.get(i).applicationInfo.metaData;
            if (metaData != null) {
                if (metaData.get("lspatch") != null || metaData.get("xposedmodule") != null ||
                        metaData.get("xposeddescription") != null || metaData.get("xposedminversion") != null ||
                        metaData.get("jshook") != null) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean checkStack() {
        StackTraceElement[] elements = Thread.currentThread().getStackTrace();
        for (StackTraceElement element : elements) {
            if (element.getClassName().contains("xposed") || element.getClassName().contains("EdHooker")) {
                return true;
            }
        }
        return false;
    }

    public String checkXposed() {
        // so 层检测
        String result = soCheckXposed();

        // java 层检测
        if (checkMemory()) {
            if (!result.isEmpty()) result += "\n";
            Log.i(TAG, "Detected Xposed!!!(Java)");
            Log.i(TAG, "Memory Detection");
            result += "Memory Detection";
        }
        if (checkStack()) {
            if (!result.isEmpty()) result += "\n";
            Log.i(TAG, "Detected Xposed!!!(Java)");
            Log.i(TAG, "StackTrace Detection");
            result += "StackTrace Detection(Java)";
        }
        if (checkPmBundle(checkHookActivity)) {
            if (!result.isEmpty()) result += "\n";
            Log.i(TAG, "Detected Xposed!!!(Java)");
            Log.i(TAG, "PackageName Detection");
            result += "PackageName Detection";
        }
        if (result.isEmpty()) {
            result = "No Xposed Detection";
        }
        return result;
    }

    public native String soCheckXposed();
}
