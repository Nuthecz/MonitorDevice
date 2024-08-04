package com.example.monitordevice;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.example.monitordevice.databinding.ActivityMainBinding;

public class MainActivity extends BaseActivity {

    static {
        System.loadLibrary("monitordevice");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG,"this is MainActivity");
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 设置标题
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        // 设置按钮跳转到检测 Root 的 Activity
        Button rootCheck = binding.rootcheck;
        rootCheck.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(MainActivity.this, "You Click the rootCheck Activity", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the rootCheck Activity");
                Intent intent = new Intent(MainActivity.this, CheckRootActivity.class);
                startActivity(intent);
            }
        });

        // 设置按钮跳转到检测 Frida 的 Activity
        Button fridaCheck = binding.fridacheck;
        fridaCheck.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(MainActivity.this, "You Click the fridaCheck Activity", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the fridaCheck Activity");
                Intent intent = new Intent(MainActivity.this, CheckFridaActivity.class);
                startActivity(intent);
            }
        });
    }

    /**
     * A native method that is implemented by the 'monitordevice' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}