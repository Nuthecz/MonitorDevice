package com.example.checkhook;

import com.example.monitordevice.CheckHookActivity;

public class HookCheck {
    protected CheckHookActivity checkHookActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("checkhook");
    }

    public HookCheck(CheckHookActivity checkHookActivity){
        this.checkHookActivity = checkHookActivity;
    }

    public native String checkHook();
}
