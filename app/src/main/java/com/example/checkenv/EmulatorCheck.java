package com.example.checkenv;

import android.content.Context;
import android.content.pm.PackageManager;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Build;
import android.util.Log;

import com.example.monitordevice.CheckEnvActivity;

import java.util.Objects;

public class EmulatorCheck extends EnvCheck {
    public EmulatorCheck(CheckEnvActivity checkEnvActivity) {
        super(checkEnvActivity);
    }

    // 检测 build 信息
    private String detectionArchitecture(String ret) {
        // 利用 build 信息检测模拟器，为模拟器会返回 true
        boolean isEmulator = Build.FINGERPRINT.startsWith("generic") || Build.FINGERPRINT.startsWith("unknown") || Build.MODEL.contains("google_sdk") || Build.MODEL.contains("Emulator") || Build.MODEL.contains("Android SDK built for x86") || Build.MANUFACTURER.contains("Genymotion") || Build.HOST.startsWith("Build") || Objects.equals(Build.PRODUCT, "google_sdk");
        if (isEmulator) {
            if (!ret.isEmpty()) ret += '\n';
            Log.i(TAG, "Detected Emu!!!(Java)");
            Log.i(TAG, "Build Info Detected");
            ret += "Build Info Detected";
        }
        return ret;
    }

    public native String checkEmu();

    // 判断是否存在指定硬件
    private String detectionHardware(Context context, String ret) {
        boolean isEmulator = false;
        PackageManager pm = context.getPackageManager();

        // 检查系统特征
        String[] features = {
                PackageManager.FEATURE_BLUETOOTH,
                PackageManager.FEATURE_CAMERA_FLASH,
                PackageManager.FEATURE_TELEPHONY
        };
        for (String feature : features) {
            if (!pm.hasSystemFeature(feature)) {
                isEmulator = true;
            }
        }

        // 检查已安装的应用
        String[] emuPkgs = {
                "com.google.android.launcher.layouts.genymotion",
                "com.bluestacks",
                "com.bignox.app"
        };
        for (String pkg : emuPkgs) {
            try {
                // 获取包名成功才会继续，否则转入 catch 块中
                pm.getPackageInfo(pkg, 0);
                isEmulator = true;
            } catch (PackageManager.NameNotFoundException e) {

            }
        }

        // 检测传感器
        SensorManager sensor = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        int sensorSize = sensor.getSensorList(Sensor.TYPE_ALL).size();
        for (int i = 0; i < sensorSize; i++) {
            Sensor s = sensor.getDefaultSensor(i);
            if (s != null && s.getName().contains("Goldfish")) {
                isEmulator = true;
            }
        }
        if (isEmulator) {
            if (!ret.isEmpty()) ret += '\n';
            Log.i(TAG, "Detected Emu!!!(Java)");
            Log.i(TAG, "Hardware Detected");
            ret += "Hardware Detected";
        }
        return ret;
    }

    public String detectionEmu() {
        // so 层检测
        String ret = checkEmu();
        // java 层检测
        ret = detectionArchitecture(ret);
        ret = detectionHardware(checkEnvActivity, ret);
        return ret;
    }
}
