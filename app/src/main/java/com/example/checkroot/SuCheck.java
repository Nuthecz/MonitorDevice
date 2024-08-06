package com.example.checkroot;

import android.util.Log;

import com.example.monitordevice.CheckRootActivity;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

// 检测 su 命令检测 root
public class SuCheck extends RootCheck{
    public SuCheck(CheckRootActivity checkRootActivity){
        super(checkRootActivity);
    }

    private boolean checkExistSu() {
        Process process = null;
        try {
            // 执行 which su
            Process whichSu = Runtime.getRuntime().exec(new String[]{"which", "su"});
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(whichSu.getInputStream()));
            return bufferedReader.readLine() != null;
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
            return false;
        } finally {
            if (process != null)
                process.destroy();
        }
    }

    public boolean check() {
        // 检测 su 是否存在
        if (checkExistSu()) {
            Log.i(TAG, "Detected Root!!!(Java)");
            Log.i(TAG, "The \"which su\" command exist");
            return true;
        }
        return false;
    }
}
