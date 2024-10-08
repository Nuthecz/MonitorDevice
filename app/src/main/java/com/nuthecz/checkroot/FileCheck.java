package com.nuthecz.checkroot;

import android.util.Log;

import com.nuthecz.monitordevice.CheckRootActivity;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;

// 检测非法二进制文件检测 root
public class FileCheck extends RootCheck {
    public FileCheck(CheckRootActivity checkRootActivity) {
        super(checkRootActivity);
    }

    // 敏感路径，里面若有特殊的文件可视为root
    public static final String[] targetPaths = {
            "/data/local/",
            "/data/local/bin/",
            "/data/local/xbin/",
            "/sbin/",
            "/su/bin/",
            "/system/bin/",
            "/system/bin/.ext/",
            "/system/bin/failsafe/",
            "/system/sd/xbin/",
            "/system/usr/we-need-root/",
            "/system/xbin/",
            "/cache/",
            "/data/",
            "/dev/"
    };

    public static final String[] fileNames = {
            "su",
            "magisk",
            "busybox"
    };

    private String[] getPaths() {
        // 初始化路径列表, 获取 targetPaths 和 环境变量的 PATH
        ArrayList<String> paths = new ArrayList<>(Arrays.asList(targetPaths));
        String sysPaths = System.getenv("PATH");

        // 如果 PATH 为空, 直接返回 targetPaths
        if (sysPaths == null || "".equals(sysPaths)) {
            return paths.toArray(new String[0]);
        }

        // 如果 PATH 不为空, 遍历 PATH, 添加到路径列表中再返回
        for (String path : sysPaths.split(":")) {
            if (!path.endsWith("/")) {
                path = path + '/';
            }
            if (!paths.contains(path)) {
                paths.add(path);
            }
        }
        return paths.toArray(new String[0]);
    }

    public boolean check() {
        // 获取可能的文件路径列表
        String[] pathsArray = this.getPaths();

        // 遍历所有路径, 检查可疑文件是否在这些路径中存在
        for (String path : pathsArray) {
            for (String filename : fileNames) {
                File f = new File(path, filename);
                boolean fileExists = f.exists(); // 检查文件是否存在,实际调用的是系统调用 faccessat
                if (fileExists) {
                    Log.i(TAG, "Detected Root!!!(Java)");
                    Log.i(TAG, "Illegal binary " + path + filename + " exist");
                    return true;
                }
            }
        }
        return false;
    }
}
