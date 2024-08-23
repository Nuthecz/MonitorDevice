package com.example.monitordevice;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import com.example.checkdevice.CpuGet;
import com.example.checkdevice.DeviceCheck;
import com.example.checkdevice.SerialGet;
import com.example.checkdevice.SystemGet;
import com.example.monitordevice.databinding.ActivityCheckDeviceBinding;

public class CheckDeviceActivity extends BaseActivity {

    private ActivityCheckDeviceBinding binding;
    private TextView outputDevice;
    private String retval = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "this is CheckDeviceActivity");
        binding = ActivityCheckDeviceBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        outputDevice = binding.outputDevice;
        outputDevice.setMovementMethod(new ScrollingMovementMethod());
        setupButtonClickListener();
    }

    private void setupButtonClickListener() {
        // 设置所有按钮的点击监听器
        Button[] buttons = {
                binding.checkSerial,
                binding.checkDisk,
                binding.checkSystem,
                binding.checkCpu,
                binding.checkKernel,
                binding.backDevice
        };

        for (Button button : buttons) {
            button.setOnClickListener(view -> handleButtonClick(button));
        }
    }

    private void handleButtonClick(Button button) {
        if (button == binding.checkSerial) {
            Log.d(TAG, "You Clicked the CheckId");
            retval = new SerialGet(this).getAndroidId();
            if (!retval.isEmpty()) {
                outputDevice.setText(retval);
            }
        } else if (button == binding.checkDisk) {
            Log.d(TAG, "You Clicked the CheckDisk");
            retval = new DeviceCheck(this).getHardDiskInfor();
            if (!retval.isEmpty()) {
                outputDevice.setText(retval);
            }
        } else if (button == binding.checkSystem) {
            Log.d(TAG, "You Clicked the CheckSystem");
            retval = new SystemGet(this).getSystemInfo();
            if (!retval.isEmpty()) {
                outputDevice.setText(retval);
            }
        } else if (button == binding.checkCpu) {
            Log.d(TAG, "You Clicked the CheckCpu");
            retval = new CpuGet(this).getCurCpuFreq();
            if (!retval.isEmpty()) {
                outputDevice.setText(retval);
            }
        } else if (button == binding.checkKernel) {
            Log.d(TAG, "You Clicked the CheckKernel");
            retval = new DeviceCheck(this).getKernelInfor();
            if (!retval.isEmpty()) {
                outputDevice.setText(retval);
            }
        } else if (button == binding.backDevice) {
            Log.i(TAG, "CheckDeviceActivity is over");
            finish();
        }
        if (retval.isEmpty()) {
            outputDevice.setText("No Device Info Find");
        }
    }
}