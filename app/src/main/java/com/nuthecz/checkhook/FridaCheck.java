package com.nuthecz.checkhook;

import com.nuthecz.monitordevice.CheckHookActivity;

public class FridaCheck {
    protected CheckHookActivity checkFridaActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("hook");
    }

    public FridaCheck(CheckHookActivity checkHookActivity) {
        this.checkFridaActivity = checkHookActivity;
    }

    public native String checkFrida();
}
