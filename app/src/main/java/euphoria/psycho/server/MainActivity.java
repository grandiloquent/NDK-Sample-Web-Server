package euphoria.psycho.server;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends Activity {


    public static final String ACTION_RUNNING = MainActivity.class.getCanonicalName() + ".ACTION_RUNNING";
    public static final boolean DEBUG = true;
    public static final String DIRECTORY_STATIC = "www";
    public static final String EXTRA_URL = "url";
    public static final String FILENAME_DATABASE = "notes_notepad.db";
    public static final int PERMISSION_REQUEST_CODE = 1;
    public static final Receiver mReceiver = new Receiver();
    private static final String TAG = "TAG/" + MainActivity.class.getSimpleName();

    static {
        System.loadLibrary("native-lib");
    }

    private void actionStratServer() {
        Intent intent = new Intent(this, ServerService.class);
        startService(intent);
    }

    private void copyAssets() {
        File targetDirectory = getAssetDirectory();
        Utilities.createDirectoryIfNotExist(targetDirectory);

        try {
            AssetManager manager = getAssets();
            String fileNames[] = manager.list("www");
            if (fileNames != null) {
                for (String fileName : fileNames) {

                    File targetFile = new File(targetDirectory, fileName);
                    if (targetFile.isFile()) continue;
                    InputStream descriptor = manager.open("www/" + fileName);
                    copyFile(descriptor, targetFile);

                }
            }
        } catch (IOException e) {
            Log.e(TAG, "copyAssets: ", e);
        }
    }

    public File getAssetDirectory() {

        return new File(Environment.getExternalStorageDirectory(), "www");
    }

    private void initialize() {
        copyAssets();
        if (DEBUG) {
            Log.d(TAG, "initialize: " + Utilities.getDeviceIP(this));
        }
    }

    private static void copyFile(InputStream inputStream, File targetFile) {

        try {

            FileOutputStream outputStream = new FileOutputStream(targetFile);
            Utilities.copy(inputStream, Utilities.MB_IN_BYTES * 10, outputStream);
            Utilities.closeQuietly(inputStream);
            Utilities.closeQuietly(outputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public static native int startServer(String host, int port, String staticDirectory, String databasePath);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.INTERNET
            }, PERMISSION_REQUEST_CODE);
        } else initialize();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_options, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_server:
                actionStratServer();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        for (int i = 0; i < grantResults.length; i++) {
            if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                String permission = permissions[i];
                new AlertDialog.Builder(this)
                        .setTitle(R.string.dialog_request_permission_title)
                        .setTitle(String.format(getResources().getString(R.string.dialog_request_permission_content), permission))
                        .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                                    requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, PERMISSION_REQUEST_CODE);
                                }
                                dialog.dismiss();
                            }
                        }).show();
                return;
            }
        }
        initialize();
    }

    @Override
    protected void onStart() {
        super.onStart();
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_RUNNING);
        registerReceiver(mReceiver, filter);
        actionStratServer();
    }

    @Override
    protected void onStop() {
        super.onStop();
        unregisterReceiver(mReceiver);
    }

    private static class Receiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (DEBUG) {
                Log.d(TAG, "onReceive: ");
            }
        }
    }
}
