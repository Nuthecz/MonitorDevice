package com.example.checkdevice;

import com.example.monitordevice.CheckDeviceActivity;

public class DeviceCheck {
    protected CheckDeviceActivity checkDeviceActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("checkdevice");
    }

    public DeviceCheck(CheckDeviceActivity checkDeviceActivity) {
        this.checkDeviceActivity = checkDeviceActivity;
    }

    public native String getHardDiskInfor();

    public native String getKernelInfor();
}
