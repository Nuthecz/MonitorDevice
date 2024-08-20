package com.example.checkdevice;

import android.annotation.SuppressLint;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;

import com.example.monitordevice.CheckDeviceActivity;

public class SerialGet extends DeviceCheck {
    public SerialGet(CheckDeviceActivity checkDeviceActivity) {
        super(checkDeviceActivity);
    }

    public String getSerialNumbers1() {
        @SuppressLint("HardwareIds")
        String serial = Settings.Secure.getString(checkDeviceActivity.getContentResolver(), Settings.Secure.ANDROID_ID);
        return serial;
    }

    private String getSerialNumbers2() {
        try {
            Bundle callResult = checkDeviceActivity.getContentResolver().call(
                    Uri.parse("content://settings/secure"), "GET_secure", "android_id", new Bundle()
            );
            assert callResult != null;
            return callResult.getString("value");
        } catch (Exception e) {
            Log.e(TAG, e.toString());
        }
        return null;
    }


    public String getAndroidId() {
        String serial1 = getSerialNumbers1();
        String serial2 = getSerialNumbers2();

        if (serial1.equals(serial2)) {
            Log.i(TAG, "Serial: " + serial1);
            return serial1;
        } else {
            return "Android Id Get Wrong";
        }
    }
}
