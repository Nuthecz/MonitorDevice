package com.example.checkenv;

import android.app.Activity;
import android.util.Log;

import com.example.checkhook.getClassLoader;
import com.example.monitordevice.CheckDeviceActivity;
import com.example.monitordevice.CheckEnvActivity;
import com.example.monitordevice.CheckHookActivity;
import com.example.monitordevice.CheckRootActivity;
import com.example.monitordevice.MainActivity;

import java.util.ArrayList;

public class SandboxCheck extends EnvCheck {
    public SandboxCheck(CheckEnvActivity checkEnvActivity) {
        super(checkEnvActivity);
    }

    public native String checkSandboxProcess();

    public static boolean checkSandboxMemory() {
        ArrayList<Activity> choose = getClassLoader.choose(Activity.class, true);
        if (choose != null) {
            ArrayList<Activity> list = new ArrayList<>();
            //移除我们的Activity,把其他的activity实例加到List里面
            for (Activity activty : choose) {
                String name = activty.getClass().getName();
                if (!name.equals(MainActivity.class.getName()) &&
                        !name.equals(CheckDeviceActivity.class.getName()) &&
                        !name.equals(CheckEnvActivity.class.getName()) &&
                        !name.equals(CheckHookActivity.class.getName()) &&
                        !name.equals(CheckRootActivity.class.getName())
                ) {
                    list.add(activty);
                }
            }
            return !list.isEmpty();
        }
        return false;
    }

    public String detectionSandbox() {
        String retval = checkSandboxProcess();
        if (checkSandboxMemory()) {
            if (!retval.isEmpty()) retval += "\n";
            Log.i(TAG, "Detected Sandbox!!!(Java)");
            Log.i(TAG, "Sandbox Detected");
            retval += "Sandbox Memory Detected";
        }
        return retval;
    }
}
