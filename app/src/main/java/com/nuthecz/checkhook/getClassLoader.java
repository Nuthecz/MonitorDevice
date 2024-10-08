package com.nuthecz.checkhook;

import android.annotation.TargetApi;
import android.util.Log;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class getClassLoader {

    protected final static String TAG = "nuthecz";
    private static final Method startMethodTracingMethod;
    private static final Method stopMethodTracingMethod;
    private static final Method getMethodTracingModeMethod;
    private static final Method getRuntimeStatMethod;
    private static final Method getRuntimeStatsMethod;
    private static final Method countInstancesOfClassMethod;
    private static final Method countInstancesOfClassesMethod;

    private static Method getInstancesOfClassesMethod;

    static {
        try {
            Class<?> c = Class.forName("dalvik.system.VMDebug");
            // 启动方法跟踪器  Integer.TYPE 和 int.class 是等价的
            startMethodTracingMethod = c.getDeclaredMethod("startMethodTracing", String.class, Integer.TYPE, Integer.TYPE, Boolean.TYPE, Integer.TYPE);
            // 停止方法跟踪器
            stopMethodTracingMethod = c.getDeclaredMethod("stopMethodTracing");
            // 获取方法跟踪器状态
            getMethodTracingModeMethod = c.getDeclaredMethod("getMethodTracingMode");
            getRuntimeStatMethod = c.getDeclaredMethod("getRuntimeStat", String.class);
            getRuntimeStatsMethod = c.getDeclaredMethod("getRuntimeStats");
            countInstancesOfClassMethod = c.getDeclaredMethod("countInstancesOfClass", Class.class, Boolean.TYPE);
            countInstancesOfClassesMethod = c.getDeclaredMethod("countInstancesOfClasses", Class[].class, Boolean.TYPE);

            if (android.os.Build.VERSION.SDK_INT >= 28) {
                getInstancesOfClassesMethod = c.getDeclaredMethod("getInstancesOfClasses", Class[].class, Boolean.TYPE);
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * 根据Class获取当前进程全部的实例
     *
     * @param clazz      需要查找的Class
     * @param assignable 是否包含子类的实例
     * @return 当前进程的全部实例。
     */
    @TargetApi(28)
    public static synchronized <T> ArrayList<T> choose(Class<T> clazz, boolean assignable) {
        ArrayList<T> resut = null;
        try {
            //从类加载器 ClassLoader.class 中查找他的子实例
            Object[][] instancesOfClasses = getInstancesOfClasses(new Class[]{clazz}, assignable);
            if (instancesOfClasses != null) {
                resut = new ArrayList<>();
                for (Object[] instancesOfClass : instancesOfClasses) {
                    List<T> objects = (List<T>) Arrays.asList(instancesOfClass);
                    resut.addAll(objects);
                }
            }
        } catch (Throwable e) {
            Log.e(TAG, "ChooseUtils choose error ", e);
            throw new RuntimeException(e);
        }
        return resut;
    }

    /**
     * @param classes    ClassLoader 类对象
     * @param assignable 是否获取所有子类的实例
     * @return 返回指定类及其子类（如果有的话）的实例
     * @throws Exception 抛出异常
     */
    @TargetApi(28)
    private static Object[][] getInstancesOfClasses(Class<?>[] classes, boolean assignable)
            throws Exception {
        return (Object[][]) getInstancesOfClassesMethod.invoke(null, classes, assignable);
    }
}
