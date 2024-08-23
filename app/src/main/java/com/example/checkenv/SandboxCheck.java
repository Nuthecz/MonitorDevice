package com.example.checkenv;

import com.example.monitordevice.CheckEnvActivity;

public class SandboxCheck extends EnvCheck {
    public SandboxCheck(CheckEnvActivity checkEnvActivity) {
        super(checkEnvActivity);
    }

    public native String checkSandbox();
}
