package com.example.monitordevice;

import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import com.example.checkenv.CloudCheck;
import com.example.checkenv.EmulatorCheck;
import com.example.checkenv.SandboxCheck;
import com.example.monitordevice.databinding.ActivityCheckEnvBinding;

public class CheckEnvActivity extends BaseActivity {
    private ActivityCheckEnvBinding binding;
    private TextView outputEnv;
    private String retval = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "this is CheckDeviceActivity");
        binding = ActivityCheckEnvBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        IntentFilter filter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        registerReceiver(CloudCheck.batteryInfoReceiver, filter);

        outputEnv = binding.outputEnv;
        outputEnv.setMovementMethod(new ScrollingMovementMethod());
        setupButtonClickListener();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(CloudCheck.batteryInfoReceiver);
    }


    private void setupButtonClickListener() {
        // 设置所有按钮的点击监听器
        Button[] buttons = {
                binding.checkEmu,
                binding.checkCloud,
                binding.checkSandbox,
                binding.backEnv,
        };

        for (Button button : buttons) {
            button.setOnClickListener(view -> handleButtonClick(button));
        }
    }

    private void handleButtonClick(Button button) {
        if (button == binding.checkEmu) {
            Log.d(TAG, "You Clicked the CheckEmu");
            retval = new EmulatorCheck(this).detectionEmu();
            if (!retval.isEmpty()) {
                outputEnv.setText(retval);
            }
        } else if (button == binding.checkCloud) {
            Log.d(TAG, "You Clicked the CheckCloud");
            retval = new CloudCheck(this).detectionCloud();
            if (!retval.isEmpty()) {
                outputEnv.setText(retval);
            }
        } else if (button == binding.checkSandbox) {
            Log.d(TAG, "You Clicked the CheckSandbox");
            retval = new SandboxCheck(this).detectionSandbox();
            if (!retval.isEmpty()) {
                outputEnv.setText(retval);
            }
        } else if (button == binding.backEnv) {
            Log.i(TAG, "CheckEnvActivity is over");
            finish();
        }
        if (retval.isEmpty()) {
            outputEnv.setText("Normal environment");
        }
    }
}
