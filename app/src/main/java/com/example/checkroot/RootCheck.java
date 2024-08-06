package com.example.checkroot;

import com.example.monitordevice.CheckRootActivity;
import com.example.monitordevice.databinding.ActivityCheckRootBinding;
// 父类,维持基本的信息
public class RootCheck {
    protected CheckRootActivity checkRootActivity;
    protected final static String TAG = "nuthecz";
    RootCheck(CheckRootActivity checkRootActivity){
        this.checkRootActivity = checkRootActivity;
    }
}
