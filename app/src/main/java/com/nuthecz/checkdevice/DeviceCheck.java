package com.nuthecz.checkdevice;

import com.nuthecz.monitordevice.CheckDeviceActivity;

public class DeviceCheck {
    protected CheckDeviceActivity checkDeviceActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("devicecheck");
    }

    public DeviceCheck(CheckDeviceActivity checkDeviceActivity) {
        this.checkDeviceActivity = checkDeviceActivity;
    }

    public native String getHardDiskInfor();

    public native String getKernelInfor();
}
