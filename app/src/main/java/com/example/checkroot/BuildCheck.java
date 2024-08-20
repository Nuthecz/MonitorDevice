package com.example.checkroot;

import android.os.Build;
import android.util.Log;

import com.example.monitordevice.CheckRootActivity;

// 检测 build类 检测 root
public class BuildCheck extends RootCheck {
    public BuildCheck(CheckRootActivity checkRootActivity) {
        super(checkRootActivity);
    }

    private String buildtag = Build.TAGS;
    private String buildfinger = Build.FINGERPRINT;

    private boolean checkBuild() {
        if (buildtag.equals("test-keys") || buildfinger.contains("userdebug"))
            return true;
        return false;
    }

    public boolean check() {
        if (checkBuild()) {
            Log.i(TAG, "Detected Root!!!(Java)");
            Log.i(TAG, "build.TAGS:" + buildtag + " build.FINGERPRINT: " + buildfinger);
            return true;
        }
        return false;
    }
}
