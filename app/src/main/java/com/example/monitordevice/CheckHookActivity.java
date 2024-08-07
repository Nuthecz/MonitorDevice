package com.example.monitordevice;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import com.example.checkhook.HookCheck;
import com.example.monitordevice.databinding.ActivityCheckHookBinding;

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

        Button checkHook = binding.checkHook;
        checkHook.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Log.d(TAG, "You Clicked the CheckHook");
                HookCheck hookCheck = new HookCheck(CheckHookActivity.this);
                String retval = hookCheck.checkHook();
                outputHook.setText(retval);
            }
        });

        Button back = binding.backHook;
        back.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Log.d(TAG, "CheckHookActivity is over");
                finish();
            }
        });
    }
}