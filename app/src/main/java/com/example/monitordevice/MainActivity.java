package com.example.monitordevice;

import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.example.monitordevice.databinding.ActivityMainBinding;
import com.hitomi.cmlibrary.CircleMenu;
import com.hitomi.cmlibrary.OnMenuSelectedListener;

import org.lsposed.hiddenapibypass.HiddenApiBypass;

public class MainActivity extends BaseActivity {

    private ActivityMainBinding binding;
    private CircleMenu circleMenu;
    private ConstraintLayout constraintLayout;
    private Intent intent;
    private final Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "this is MainActivity");
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 绕过隐藏 API 限制
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            HiddenApiBypass.addHiddenApiExemptions("L");
        }

        circleMenu = binding.circleMenuMain;
        constraintLayout = binding.constraintLayout;
        circleMenu.setMainMenu(Color.parseColor("#CDCDCD"), R.drawable.icon_menu, R.drawable.icon_cancel)
                .addSubMenu(Color.parseColor("#88bef5"), R.drawable.icon_root)
                .addSubMenu(Color.parseColor("#83e85a"), R.drawable.icon_hook)
                .addSubMenu(Color.parseColor("#FF4B32"), R.drawable.icon_phone)
                .addSubMenu(Color.parseColor("#ba53de"), R.drawable.icon_env)
                .setOnMenuSelectedListener(new OnMenuSelectedListener() {
                    @Override
                    public void onMenuSelected(int index) {
                        switch (index) {
                            case 0:
                                Toast.makeText(MainActivity.this, "You Click the CheckRoot Activity", Toast.LENGTH_SHORT).show();
                                Log.d(TAG, "You Click the CheckRootActivity");
                                constraintLayout.setBackgroundColor(Color.parseColor("#ecfffb"));
                                intent = new Intent(MainActivity.this, CheckRootActivity.class);
                                break;
                            case 1:
                                Toast.makeText(MainActivity.this, "You Click the CheckHook Activity", Toast.LENGTH_SHORT).show();
                                constraintLayout.setBackgroundColor(Color.parseColor("#96f7d2"));
                                Log.d(TAG, "You Click the CheckHookActivity");
                                intent = new Intent(MainActivity.this, CheckHookActivity.class);
                                break;
                            case 2:
                                Toast.makeText(MainActivity.this, "You Click the CheckDevice Activity", Toast.LENGTH_SHORT).show();
                                constraintLayout.setBackgroundColor(Color.parseColor("#fac4a2"));
                                Log.d(TAG, "You Click the CheckDeviceActivity");
                                intent = new Intent(MainActivity.this, CheckDeviceActivity.class);
                                break;
                            case 3:
                                Toast.makeText(MainActivity.this, "You Click the CheckEnv Activity", Toast.LENGTH_SHORT).show();
                                constraintLayout.setBackgroundColor(Color.parseColor("#d3cde6"));
                                Log.d(TAG, "You Click the CheckEnvActivity");
                                intent = new Intent(MainActivity.this, CheckEnvActivity.class);
                                break;
                        }
                        handler.postDelayed(() -> {
                            startActivity(intent);
                            constraintLayout.setBackgroundColor(Color.parseColor("#FFFFFF"));
                        }, 1000);
                    }
                });
    }
}