package com.nuthecz.monitordevice;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import com.example.monitordevice.databinding.ActivityCheckRootBinding;
import com.nuthecz.checkroot.BuildCheck;
import com.nuthecz.checkroot.FileCheck;
import com.nuthecz.checkroot.SuCheck;
import com.nuthecz.checkroot.WritePermissionCheck;

// 检测 Root 的 Activity
public class CheckRootActivity extends BaseActivity {

    private ActivityCheckRootBinding binding;
    private TextView outputRoot;
    private boolean flag = false;

    static {
        System.loadLibrary("devicecheck");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "this is CheckRootActivity");

        binding = ActivityCheckRootBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        outputRoot = binding.outputRoot;
        setupButtonClickListener();
    }

    private void setupButtonClickListener() {
        // 设置所有按钮的点击监听器
        Button[] buttons = {
                binding.checkSu,
                binding.checkFile,
                binding.checkWritePermission,
                binding.checkBuild,
                binding.checkUseSo,
                binding.backRoot
        };

        for (Button button : buttons) {
            button.setOnClickListener(view -> handleButtonClick(button));
        }
    }

    private void handleButtonClick(Button button) {
        if (button == binding.checkSu) {
            Log.d(TAG, "You Clicked the CheckSu");
            flag = new SuCheck(this).check();
            if (flag) {
                outputRoot.setText("The \"which su\" command exist");
            }
        } else if (button == binding.checkFile) {
            Log.d(TAG, "You Clicked the CheckFile");
            flag = new FileCheck(this).check();
            if (flag) {
                outputRoot.setText("Illegal binary exist");
            }
        } else if (button == binding.checkWritePermission) {
            Log.d(TAG, "You Clicked the checkWritePermission");
            flag = new WritePermissionCheck(this).check();
            if (flag) {
                outputRoot.setText("There is a path with rw permissions");
            }
        } else if (button == binding.checkBuild) {
            Log.d(TAG, "You Clicked the checkBuild");
            flag = new BuildCheck(this).check();
            if (flag) {
                outputRoot.setText("build Detect Root");
            }
        } else if (button == binding.checkUseSo) {
            Log.d(TAG, "You Clicked the checkUseSo");
            flag = checkUseso(FileCheck.targetPaths, FileCheck.fileNames);
            if (flag) {
                outputRoot.setText("Illegal binary exist");
            }
        } else if (button == binding.backRoot) {
            Log.i(TAG, "CheckRootActivity is over");
            finish();
        }

        if (!flag) {
            outputRoot.setText("No Root Detected");
        }
    }

    public native boolean checkUseso(String[] targetPaths, String[] fileNames);
}
