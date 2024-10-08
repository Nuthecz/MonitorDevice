package com.nuthecz.checkdevice;

import android.annotation.SuppressLint;
import android.os.Build;
import android.util.Log;

import com.nuthecz.monitordevice.CheckDeviceActivity;

import org.lsposed.hiddenapibypass.HiddenApiBypass;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class SystemGet extends DeviceCheck {
    public SystemGet(CheckDeviceActivity checkDeviceActivity) {
        super(checkDeviceActivity);
    }

    private String getBuildInfo() {
        StringBuilder infoBuilder = new StringBuilder();

        infoBuilder.append("(").append(Build.MODEL); // 设备型号名称
        infoBuilder.append("/").append(Build.BRAND); // 设备品牌名称
        // 当前运行的 Android SDK 版本的整数表示。API Level 28 表示 Android 9
        infoBuilder.append(")--SDK: ").append(Build.VERSION.SDK_INT).append("\n");

        //  设备的完整指纹字符串，通常包括制造商、品牌、设备型号和构建类型等信息。
        infoBuilder.append(Build.FINGERPRINT).append("\n");
        // infoBuilder.append(Build.getRadioVersion()).append("\n");

        Log.i(TAG, infoBuilder.toString());
        return infoBuilder.toString();
    }

    // 获取基带版本，和 Build.getRadioVersion() 方法效果相同，但后者在 API 24 及以上版本可用
    // 基带版本信息指的是手机调制解调器（也称为基带处理器或基带芯片）所使用的固件或驱动程序的版本号。
    // 调制解调器负责处理无线通信信号，使手机能够进行语音通话、发送短信以及连接到移动数据网络。
    private String getBaseband() {
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

    public String getSystemInfo() {
        return getBuildInfo() + getBaseband();
    }
}
