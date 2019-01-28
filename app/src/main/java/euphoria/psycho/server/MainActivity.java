package euphoria.psycho.server;

import android.app.Activity;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends Activity {


    static {
        System.loadLibrary("native-lib");
    }


    private void copyAssets() {
        File targetDirectory = getAssetDirectory();
        Utilities.createDirectoryIfNotExist(targetDirectory);

        try {
            AssetManager manager = getAssets();
            String fileNames[] = manager.list(".");
            if (fileNames != null) {
                for (String fileName : fileNames) {
                    AssetFileDescriptor descriptor = manager.openFd(fileName);
                    copyFile(descriptor, new File(targetDirectory, fileName));

                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public File getAssetDirectory() {

        return null;
    }

    private void initialize() {

    }

    private static void copyFile(AssetFileDescriptor descriptor, File targetFile) {
        if (targetFile.isFile()) {
            if (descriptor.getLength() == targetFile.length()) return;
            else {
                if (!targetFile.delete()) return;
            }
        }
        try {
            InputStream inputStream = descriptor.createInputStream();
            FileOutputStream outputStream = new FileOutputStream(targetFile);
            Utilities.copy(inputStream, Utilities.MB_IN_BYTES * 10, outputStream);
            Utilities.closeQuietly(inputStream);
            Utilities.closeQuietly(outputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public static native String startServer(String staticDirectory);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }
}
