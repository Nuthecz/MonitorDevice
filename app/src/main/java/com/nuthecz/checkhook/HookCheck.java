package com.nuthecz.checkhook;

import com.nuthecz.monitordevice.CheckHookActivity;

public class HookCheck {
    protected CheckHookActivity checkHookActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("hook");
    }

    public HookCheck(CheckHookActivity checkHookActivity) {
        this.checkHookActivity = checkHookActivity;
    }

    public native String checkHook();
}
