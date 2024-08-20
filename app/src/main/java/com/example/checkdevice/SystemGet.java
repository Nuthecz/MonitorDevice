package com.example.checkdevice;

import android.annotation.SuppressLint;
import android.os.Build;
import android.util.Log;

import com.example.monitordevice.CheckDeviceActivity;

import org.lsposed.hiddenapibypass.HiddenApiBypass;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class SystemGet extends DeviceCheck {
    public SystemGet(CheckDeviceActivity checkDeviceActivity) {
        super(checkDeviceActivity);
    }

    public String getBuildInfo() {
        StringBuilder infoBuilder = new StringBuilder();

        infoBuilder.append("(").append(Build.MODEL);
        infoBuilder.append("/").append(Build.BRAND);
        infoBuilder.append(")--SDK: ").append(Build.VERSION.SDK_INT).append("\n");

        infoBuilder.append(Build.FINGERPRINT).append("\n");

//        infoBuilder.append(Build.getRadioVersion()).append("\n"); // 注意：Build.RADIO 在API 24及以上可用

        Log.i(TAG, infoBuilder.toString());
        return infoBuilder.toString();
    }

    // 获取基带版本，和 Build.getRadioVersion() 方法效果相同
    public String getBaseband() {
        String retval = null;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            try {
                @SuppressLint("PrivateApi")
                Class<?> clazz = Class.forName("android.os.SystemProperties");
                Object invoker = HiddenApiBypass.newInstance(clazz);
                Object result = HiddenApiBypass.invoke(clazz, invoker, "get", "gsm.version.baseband", "no message");
                retval = (String) result;
                Log.i(TAG, (String) result);
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
        } else {
            try {
                @SuppressLint("PrivateApi")
                Class<?> clazz = Class.forName("android.os.SystemProperties");
                Object invoker = clazz.newInstance();
                Method get = clazz.getDeclaredMethod("get", String.class, String.class);
                Object result = get.invoke(invoker, "gsm.version.baseband", "no message");
                assert result != null;
                Log.i(TAG, (String) result);
                retval = (String) result;

            } catch (ClassNotFoundException | InstantiationException | IllegalAccessException |
                     InvocationTargetException | NoSuchMethodException e) {
                e.printStackTrace();
            }
        }
        return retval;
    }

}
