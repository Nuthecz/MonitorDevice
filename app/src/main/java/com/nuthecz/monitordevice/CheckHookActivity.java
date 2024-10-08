package com.nuthecz.monitordevice;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import com.example.monitordevice.databinding.ActivityCheckHookBinding;
import com.nuthecz.checkhook.FridaCheck;
import com.nuthecz.checkhook.HookCheck;
import com.nuthecz.checkhook.XposedCheck;

public class CheckHookActivity extends BaseActivity {

    private ActivityCheckHookBinding binding;
    private TextView outputHook;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "this is CheckHookActivity");

        binding = ActivityCheckHookBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        outputHook = binding.outputHook;
        setupButtonClickListener();

    }

    private void setupButtonClickListener() {
        // 设置所有按钮的点击监听器
        Button[] buttons = {
                binding.checkHook,
                binding.checkFrida,
                binding.checkXposed,
                binding.backHook
        };
        for (Button button : buttons) {
            button.setOnClickListener(view -> handleButtonClick(button));
        }
    }

    private void handleButtonClick(Button button) {
        if (button == binding.checkFrida) {
            Log.d(TAG, "You Clicked the CheckFrida");
            String fridaRet = new FridaCheck(this).checkFrida();
            outputHook.setText(fridaRet);
        } else if (button == binding.checkHook) {
            Log.d(TAG, "You Clicked the CheckHook");
            String hookRet = new HookCheck(this).checkHook();
            outputHook.setText(hookRet);
        } else if (button == binding.checkXposed) {
            Log.d(TAG, "You Clicked the CheckXposed");
            String xposedRet = new XposedCheck(this).checkXposed();
            outputHook.setText(xposedRet);
        } else if (button == binding.backHook) {
            Log.i(TAG, "CheckHookActivity is over");
            finish();
        }
    }
}