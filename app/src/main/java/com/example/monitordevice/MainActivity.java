package com.example.monitordevice;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;
import android.content.Intent;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.example.monitordevice.databinding.ActivityMainBinding;
import com.hitomi.cmlibrary.CircleMenu;
import com.hitomi.cmlibrary.OnMenuSelectedListener;

public class MainActivity extends BaseActivity {

    private ActivityMainBinding binding;
    private CircleMenu circleMenu;
    private ConstraintLayout constraintLayout;
    private Intent intent;
    private final Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG,"this is MainActivity");
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        circleMenu = binding.circleMenuMain;
        constraintLayout = binding.constraintLayout;
        circleMenu.setMainMenu(Color.parseColor("#CDCDCD"),R.drawable.icon_menu, R.drawable.icon_cancel)
                .addSubMenu(Color.parseColor("#88bef5"),R.drawable.icon_root)
                .addSubMenu(Color.parseColor("#ff8a5c"),R.drawable.icon_frida)
                .setOnMenuSelectedListener(new OnMenuSelectedListener() {
                    @Override
                    public void onMenuSelected(int index) {
                        switch (index) {
                            case 0:
                                Toast.makeText(MainActivity.this, "You Click the rootCheck Activity", Toast.LENGTH_SHORT).show();
                                Log.d(TAG, "You Click the rootCheck Activity");
                                constraintLayout.setBackgroundColor(Color.parseColor("#88bef5"));
                                intent = new Intent(MainActivity.this, CheckRootActivity.class);

                                break;
                            case 1:
                                Toast.makeText(MainActivity.this, "You Click the fridaCheck Activity", Toast.LENGTH_SHORT).show();
                                constraintLayout.setBackgroundColor(Color.parseColor("#ff8a5c"));
                                Log.d(TAG, "You Click the fridaCheck Activity");
                                intent = new Intent(MainActivity.this, CheckFridaActivity.class);
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