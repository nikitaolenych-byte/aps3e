// SPDX-License-Identifier: WTFPL

package aenu.aps3e;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.io.ByteArrayOutputStream;
import android.*;
import java.util.*;
import android.content.pm.*;
import android.content.*;
import android.app.*;
import java.io.*;
import android.content.res.*;

import aenu.hardware.ProcessorInfo;

public class Application extends android.app.Application
{
	
	public static String getCurrentProcessName(Context context) {
        int pid = android.os.Process.myPid();
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningAppProcessInfo> runningApps = am.getRunningAppProcesses();
        if (runningApps!= null) {
            for (ActivityManager.RunningAppProcessInfo procInfo : runningApps) {
                if (procInfo.pid == pid) {
                    return procInfo.processName;
                }
            }
        }
        return null;
    }
	
	public static void extractAssetsDir(Context context, String assertDir, File outputDir) {
        AssetManager assetManager = context.getAssets();
        try {
            if (!outputDir.exists()) {
                outputDir.mkdirs();
            }

            String[] filesToExtract = assetManager.list(assertDir);
            if (filesToExtract!= null) {
                for (String file : filesToExtract) {
					File outputFile = new File(outputDir, file);
					if(outputFile.exists())continue;
					
                    InputStream in = assetManager.open(assertDir + "/" + file);
                    FileOutputStream out = new FileOutputStream(outputFile);
                    byte[] buffer = new byte[16384];
                    int read;
                    while ((read = in.read(buffer))!= -1) {
                        out.write(buffer, 0, read);
                    }
                    in.close();
                    out.close();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public  static byte[] load_assets_file(Context ctx,String asset_file_path) {
        try {
            InputStream in = ctx.getAssets().open(asset_file_path);
            int size = in.available();
            byte[] buffer = new byte[size];
            in.read(buffer);
            in.close();
            return buffer;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    public  static File get_app_data_dir() {
        return ctx.getExternalFilesDir("aps3e");
    }

    public  static File get_app_log_dir() {
        return new File(get_app_data_dir(),"logs");
    }

    public static String get_native_lib_dir()
    {
        return ctx.getApplicationInfo().nativeLibraryDir;
    }
    public static File get_internal_data_dir()
    {
        return new File(ctx.getApplicationInfo().dataDir,"aps3e");
    }

    public static File get_custom_font_dir()
    {
        return new File(get_app_data_dir(),"font");
    }

    //sdcardfs文件系统无法创建可执行文件，只能放在内部存储(ext4)
    public static File get_custom_driver_dir()
    {
        return new File(get_internal_data_dir(),"aps3e/driver");
    }

    public static File get_custom_cfg_dir()
    {
        return new File(get_app_data_dir(),"config/custom_cfg");
    }

    public static File get_custom_cfg_file(String serial)
    {
        return new File(get_custom_cfg_dir(),serial+".yml");
    }
    public static File get_global_config_file(){
        return new File(Application.get_app_data_dir(),"config/config.yml");
    }

    public static File get_default_config_file(){
        return new File(Application.get_app_data_dir(),"config/default_config.yml");
    }

    static void copy_file(File src,File dst) {
        try {
            FileInputStream in=new FileInputStream(src);
            FileOutputStream out=new FileOutputStream(dst);
            byte buf[]=new byte[16384];
            int n;
            while((n=in.read(buf))!=-1)
                out.write(buf,0,n);
            in.close();
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static boolean device_support_vulkan() {
        return gpu_device_name_vk!=null;
    }

    static boolean should_delay_load() {
        if(gpu_device_name_vk==null)
            throw new RuntimeException("gpu_device_name_vk==null");
        return gpu_device_name_vk.contains("Adreno (TM) 5")
                || gpu_device_name_vk.contains("Adreno (TM) 6");
    }

    public  static Context ctx;
    public static String gpu_device_name_vk;
    @Override
    public void onCreate()
    {
        super.onCreate();

        Application.ctx=this;
        gpu_device_name_vk= ProcessorInfo.gpu_get_physical_device_name_vk();

        Emulator.setup_preload_env(this);
        if(!should_delay_load())
            Emulator.load_library();

        //get_app_data_dir().mkdirs();
        get_app_log_dir().mkdirs();
        //get_internal_data_dir().mkdirs();
        get_custom_driver_dir().mkdirs();

		if(getPackageName().equals(getCurrentProcessName(this)));
			//Logger.start_record(this);

		Thread.setDefaultUncaughtExceptionHandler(exception_handler);
    }

    private static ExceptionHandler exception_handler=new ExceptionHandler();

    private static class ExceptionHandler implements Thread.UncaughtExceptionHandler{

        @Override
        public void uncaughtException(Thread p1, Throwable p2){
            try
            {
                ByteArrayOutputStream err=new ByteArrayOutputStream();
                PrintStream print=new PrintStream(err);
                p2.printStackTrace(print);
                android.util.Log.e("aps3e_java",err.toString());
            }
            catch (Exception e)
            {}
        }

        
    }
}
