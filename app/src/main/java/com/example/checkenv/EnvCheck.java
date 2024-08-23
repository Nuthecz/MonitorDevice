package com.example.checkenv;

import com.example.monitordevice.CheckEnvActivity;

public class EnvCheck {
    protected CheckEnvActivity checkEnvActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("checkenv");
    }

    public EnvCheck(CheckEnvActivity checkEnvActivity) {
        this.checkEnvActivity = checkEnvActivity;
    }

}
