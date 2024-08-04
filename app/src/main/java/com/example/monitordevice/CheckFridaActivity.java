package com.example.monitordevice;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.example.checkfrida.FridaCheck;
import com.example.monitordevice.databinding.ActivityCheckFridaBinding;

public class CheckFridaActivity extends BaseActivity {

    private ActivityCheckFridaBinding binding;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityCheckFridaBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 设置标题
        TextView tv = binding.checkText;
        tv.setText("checkFrida");

        Button fridaCheck = binding.checkFrida;
        fridaCheck.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(CheckFridaActivity.this, "You Click the checkFrida", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the checkFrida");
                FridaCheck fridaCheck = new FridaCheck(CheckFridaActivity.this);
                fridaCheck.checkFrida();
            }
        });
    }


}