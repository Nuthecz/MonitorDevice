package com.example.checkhook;

import com.example.monitordevice.CheckHookActivity;

public class FridaCheck {
    protected CheckHookActivity checkFridaActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("checkhook");
    }

    public FridaCheck(CheckHookActivity checkHookActivity){
        this.checkFridaActivity = checkHookActivity;
    }

    public native String checkFrida();
}
