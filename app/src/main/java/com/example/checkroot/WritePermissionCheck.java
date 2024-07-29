package com.example.checkroot;
import android.util.Log;

import com.example.monitordevice.CheckRootActivity;

import java.io.IOException;
import java.io.InputStream;
import java.util.NoSuchElementException;
import java.util.Scanner;

// 检测特殊路径是否有读写权限检测root
public class WritePermissionCheck extends RootCheck{
    public WritePermissionCheck(CheckRootActivity checkRootActivity){
        super(checkRootActivity);
    }

    private String[] NotWritePermissionPaths = {
            "/system",
            "/system/bin",
            "/system/sbin",
            "/system/xbin",
            "/vendor/bin",
            "/sbin",
            "/etc",
            "/sys",
            "/proc",
            "/dev"
    };

    private String[] mountReader() {
        try {
            InputStream inputstream = Runtime.getRuntime().exec("mount").getInputStream();
            if (inputstream == null) return null;

            // 使用正则表达式"\A"作为分隔符，一次性读取输入流的所有内容
            String propVal = new Scanner(inputstream).useDelimiter("\\A").next();
            return propVal.split("\n");
        } catch (IOException | NoSuchElementException e) {
            Log.e(TAG, e.getMessage());
            return null;
        }
    }

    public void check() {
        String[] lines = mountReader();

        if (lines == null){
            return;
        }

        int sdkVersion = android.os.Build.VERSION.SDK_INT;

        for (String line : lines) {
            String[] args = line.split(" ");
            // 检查每行信息是否包含足够的字段，旧版本的Android系统需要至少4个字段，新版本需要至少6个
            if ((sdkVersion <= android.os.Build.VERSION_CODES.M && args.length < 4)
                    || (sdkVersion > android.os.Build.VERSION_CODES.M && args.length < 6)) {
                continue;
            }

            String mountPoint;
            String mountOptions;

            // 根据Android版本确定挂载点和挂载选项的索引
            if (sdkVersion > android.os.Build.VERSION_CODES.M) {
                mountPoint = args[2];
                mountOptions = args[5];
            } else {
                mountPoint = args[1];
                mountOptions = args[3];
            }

            // 遍历所有不应具有读写权限的路径
            for(String pathToCheck: this.NotWritePermissionPaths) {
                if (mountPoint.equalsIgnoreCase(pathToCheck)) {
                    // 对于Android版本高于Marshmallow的设备，需要从挂载选项中移除括号
                    if (android.os.Build.VERSION.SDK_INT > android.os.Build.VERSION_CODES.M) {
                        mountOptions = mountOptions.replace("(", "");
                        mountOptions = mountOptions.replace(")", "");
                    }

                    // 分割挂载选项并检查是否包含"rw"，以确定该路径是否以读写模式挂载
                    for (String option : mountOptions.split(",")){
                        if (option.equalsIgnoreCase("rw")){
                            // 如果路径以读写模式挂载，显示警告信息
                            Log.i(TAG, "Detected Root!!!(Java)");
                            Log.i(TAG, pathToCheck + " The path is mounted with rw permissions " + line);
                        }
                    }
                }
            }
        }
    }
}
