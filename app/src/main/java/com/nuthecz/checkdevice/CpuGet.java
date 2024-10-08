package com.nuthecz.checkdevice;

import android.util.Log;

import com.nuthecz.monitordevice.CheckDeviceActivity;

import java.io.IOException;
import java.io.InputStream;

public class CpuGet extends DeviceCheck {

    public CpuGet(CheckDeviceActivity checkDeviceActivity) {
        super(checkDeviceActivity);
    }

    public String getCurCpuFreq() {
        StringBuilder result = new StringBuilder();
        String[] args = {"cat", "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"};
        ProcessBuilder cmd = new ProcessBuilder(args);

        try {
            Process process = cmd.start();
            InputStream in = process.getInputStream();
            byte[] buf = new byte[16];
            while (in.read(buf) != -1) {
                result.append(new String(buf));
            }
            in.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.i(TAG, "cpu0_cur_freq：" + result.toString().trim());
        return "Cpu0 Cur Freq：" + result.toString().trim();
    }
}
