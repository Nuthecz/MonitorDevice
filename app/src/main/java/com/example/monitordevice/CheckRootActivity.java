package com.example.monitordevice;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.example.checkroot.BuildCheck;
import com.example.checkroot.FileCheck;
import com.example.checkroot.SuCheck;
import com.example.checkroot.WritePermissionCheck;
import com.example.monitordevice.databinding.ActivityCheckRootBinding;

// 检测 Root 的 Activity
public class CheckRootActivity extends BaseActivity {

    private ActivityCheckRootBinding binding;

    static {
        System.loadLibrary("checkroot");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG,"this is CheckRootActivity");

        binding = ActivityCheckRootBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 设置标题
        TextView tv = binding.checkText;
        tv.setText("checkRoot");

        // 配置利用 which su 检测 root 的方式
        Button checkSu = binding.checkSu;
        checkSu.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(CheckRootActivity.this, "You Click the CheckSu", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the CheckSu");
                SuCheck suCheck = new SuCheck(CheckRootActivity.this);
                suCheck.check();
            }
        });

        // 配置利用非法二进制文件检测 root 的方式
        Button checkFile = binding.checkFile;
        checkFile.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(CheckRootActivity.this, "You Click the CheckFile", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the CheckFile");
                FileCheck fileCheck = new FileCheck(CheckRootActivity.this);
                fileCheck.check();
            }
        });

        // 配置查看特殊路径写权限检测 root 的方式
        Button checkWritePermission = binding.checkWritePermission;
        checkWritePermission.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(CheckRootActivity.this, "You Click the checkWritePermission", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the checkWritePermission");
                WritePermissionCheck writePermissionCheck = new WritePermissionCheck(CheckRootActivity.this);
                writePermissionCheck.check();
            }
        });

        // 配置利用 build 类信息检测 root 的方式
        Button checkBuild = binding.checkBuild;
        checkBuild.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(CheckRootActivity.this, "You Click the checkBuild", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the checkBuild");
                BuildCheck buildCheck = new BuildCheck(CheckRootActivity.this);
                buildCheck.check();
            }
        });

        // 配置利用 so 文件调用系统函数检测非法文件检测 root 的方式
        Button checkUseSo = binding.checkUseSo;
        checkUseSo.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                Toast.makeText(CheckRootActivity.this, "You Click the checkUseSo", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "You Click the checkUseSo");
                checkUseso(FileCheck.targetPaths, FileCheck.fileNames);
            }
        });
    }

    public native void checkUseso(String[] targetPaths, String[] fileNames);
}