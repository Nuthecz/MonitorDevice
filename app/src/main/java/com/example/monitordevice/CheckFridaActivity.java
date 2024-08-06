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
    private TextView outputFrida;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityCheckFridaBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        outputFrida = binding.outputFrida;

        Button fridaCheck = binding.checkFrida;
        fridaCheck.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Log.d(TAG, "You Click the CheckFridaActivity");
                FridaCheck fridaCheck = new FridaCheck(CheckFridaActivity.this);
                String retval = fridaCheck.checkFrida();
                outputFrida.setText(retval);
            }
        });

        Button back = binding.backFrida;
        back.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Log.d(TAG, "CheckFridaActivity is over");
                finish();
            }
        });
    }
}