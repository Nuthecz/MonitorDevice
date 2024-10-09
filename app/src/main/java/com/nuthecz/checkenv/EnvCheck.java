package com.nuthecz.checkenv;

import com.nuthecz.monitordevice.CheckEnvActivity;

public class EnvCheck {
    protected CheckEnvActivity checkEnvActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("device");
    }

    public EnvCheck(CheckEnvActivity checkEnvActivity) {
        this.checkEnvActivity = checkEnvActivity;
    }

}
