package com.nuthecz.checkroot;

import com.nuthecz.monitordevice.CheckRootActivity;

// 父类,维持基本的信息
public class RootCheck {
    protected CheckRootActivity checkRootActivity;
    protected final static String TAG = "nuthecz";

    RootCheck(CheckRootActivity checkRootActivity) {
        this.checkRootActivity = checkRootActivity;
    }
}
