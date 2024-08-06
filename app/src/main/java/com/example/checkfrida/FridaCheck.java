package com.example.checkfrida;

import com.example.monitordevice.CheckFridaActivity;

public class FridaCheck {
    protected CheckFridaActivity checkFridaActivity;
    protected final static String TAG = "nuthecz";

    static {
        System.loadLibrary("checkfrida");
    }

    public FridaCheck(CheckFridaActivity checkFridaActivity){
        this.checkFridaActivity = checkFridaActivity;
    }

    public native String checkFrida();
}
