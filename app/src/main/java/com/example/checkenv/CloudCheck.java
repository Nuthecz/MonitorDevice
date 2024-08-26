package com.example.checkenv;

import static android.content.Context.SENSOR_SERVICE;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraManager;
import android.os.BatteryManager;
import android.util.Log;

import com.example.monitordevice.CheckEnvActivity;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class CloudCheck extends EnvCheck {
    public CloudCheck(CheckEnvActivity checkEnvActivity) {
        super(checkEnvActivity);
    }

    private static boolean batteryFlag = false;
    private static final int CAMERA_MINIMUM_QUANTITY_LIMIT = 2;
    private static final int SENSOR_MINIMUM_QUANTITY_LIMIT = 20;
    private static final int SENSOR_TYPE_MINIMUM_QUANTITY_LIMIT = 20;

    public static final BroadcastReceiver batteryInfoReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            // 电池状态
            int plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
            // 电压（以毫伏为单位）
            int voltage = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, -1);
            // 获取电池电流（毫安）
            int currentNow = -1;
            BatteryManager batteryManager = (BatteryManager) context.getSystemService(Context.BATTERY_SERVICE);
            currentNow = batteryManager.getIntProperty(BatteryManager.BATTERY_PROPERTY_CURRENT_NOW);

//            Log.i(TAG, "现在电量" + currentNow);
            // 判断是否在充电
            if (plugged == BatteryManager.BATTERY_PLUGGED_AC || plugged == BatteryManager.BATTERY_PLUGGED_USB || plugged == BatteryManager.BATTERY_PLUGGED_WIRELESS) {
                // 在充电
                if (voltage != -1 && currentNow != -1) {
                    float voltageInVolts = voltage / 1000f; // 将电压转换为伏特
                    float currentInAmperes = currentNow / 1000000f; // 将电流转换为安培
                    float chargingPower = voltageInVolts * currentInAmperes; // 计算充电功率（瓦特）
//                    Log.i(TAG, "充电功率: " + chargingPower);
                    // 判断充电功率是否大于300瓦，大于300瓦，则认为是云手机
                    if (Math.abs(chargingPower) > 300) {
                        batteryFlag = true;
                    }
                }
            }
        }
    };

    // 检测摄像头，少于 2 个时，认为云手机
    private boolean detectionCamera(Context context) {
        CameraManager manager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        String[] cameraIds = null;
        try {
            cameraIds = manager.getCameraIdList();
        } catch (CameraAccessException e) {
            throw new RuntimeException(e);
        }
        // 摄像头 ID
//        Log.i(TAG, "cameraIds -> " + Arrays.toString(cameraIds));
        return cameraIds.length < CAMERA_MINIMUM_QUANTITY_LIMIT;
    }

    // 检测传感器类型，少于 20 个认为是云手机
    private boolean detectionSensor(Context context) {
        boolean flag = false;
        SensorManager sm = (SensorManager) context.getSystemService(SENSOR_SERVICE);
        List<Sensor> sensorlist = sm.getSensorList(Sensor.TYPE_ALL);
        ArrayList<Integer> sensorTypeS = new ArrayList<>();
        for (Sensor sensor : sensorlist) {
            // 获取传感器类型
            int type = sensor.getType();
            if (!sensorTypeS.contains(type)) {
                // 发现一种类型则添加一种类型
                sensorTypeS.add(type);
            }
        }
        // 小米 k40 51 个传感器类型；普通的pix 27 个；华为荣耀20 18 个传感器
//        Log.e(TAG,"sensor types size -> " + sensorlist.size());
        // 这里认为传感器少于20个护着传感器类型少于 15 个，就认为是风险设备
        if (sensorlist.size() < SENSOR_MINIMUM_QUANTITY_LIMIT || sensorTypeS.size() < SENSOR_TYPE_MINIMUM_QUANTITY_LIMIT) {
            flag = true;
        }

        // 传感器名称检测
        ArrayList<Sensor> aospSensor = new ArrayList<>();
        for (Sensor sensor : sensorlist) {
            if (sensor.getVendor().contains("AOSP")) {
                aospSensor.add(sensor);
            }
        }
        // 这里检测传感器的名称，用 AOSP 基本都是自己编译的ROM，所以可以视为云手机
        if (aospSensor.size() > 3) {
            flag = true;
        }
        return flag;
    }

    private boolean detectionMounts() {
        String[] marks = {"docker"};
        String[] filesToCheck = {"/proc/mounts", "/proc/self/mountstats", "/proc/self/mountinfo"};

        for (String file : filesToCheck) {
            try {
                // 读取文件内容
                List<String> lines = Files.readAllLines(Paths.get(file), StandardCharsets.UTF_8);

                // 遍历每一行，检查是否包含关键词
                for (String line : lines) {
                    for (String mark : marks) {
                        if (line.contains(mark)) {
                            return true; // 找到关键词，返回 true
                        }
                    }
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        return false; // 没有找到关键词
    }

    public String detectionCloud() {
        String ret = "";
        if (batteryFlag) {
            Log.i(TAG, "Detected CloudPhone!!!(Java)");
            Log.i(TAG, "Battery Detected");
            ret += "Battery Detected";
        }
        if (detectionCamera(checkEnvActivity)) {
            if (!ret.isEmpty()) ret += '\n';
            Log.i(TAG, "Detected CloudPhone!!!(Java)");
            Log.i(TAG, "Camera Detected");
            ret += "Camera Detected";
        }
        if (detectionSensor(checkEnvActivity)) {
            if (!ret.isEmpty()) ret += '\n';
            Log.i(TAG, "Detected CloudPhone!!!(Java)");
            Log.i(TAG, "Sensor Detected");
            ret += "Sensor Detected";
        }
        if (detectionMounts()) {
            if (!ret.isEmpty()) ret += '\n';
            Log.i(TAG, "Detected CloudPhone!!!(Java)");
            Log.i(TAG, "Mounts Detected");
            ret += "Mounts Detected";
        }
        return ret;
    }
}
