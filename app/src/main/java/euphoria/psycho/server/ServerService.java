package euphoria.psycho.server;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.IBinder;
import android.os.PowerManager;

public class ServerService extends Service {
    public static final String CHANNEL_ID = "default";
    public static final String CHANNEL_NAME = "Web Server";
    public static final int ID = 100;
    private PowerManager.WakeLock mWakeLock;

    private Notification showPriorityNotification() {
        Notification.Builder builder = null;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            builder = new Notification.Builder(this, CHANNEL_ID);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {

            builder = new Notification.Builder(this);
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            builder.setSmallIcon(android.R.drawable.stat_sys_download)
                    .setContentTitle("Server")
                    .setContentText("Running the Server");
            return builder.build();
        } else {
            return new Notification();
        }

    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        PowerManager manager = (PowerManager) getSystemService(POWER_SERVICE);
        if (manager != null) {
            mWakeLock = manager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, ServerService.class.getSimpleName());
        }
        mWakeLock.acquire(10*60*1000L /*10 minutes*/);
        Utilities.createNotificationChannel(this, CHANNEL_ID, CHANNEL_NAME);
        startForeground(ID, showPriorityNotification());
    }

    @Override
    public void onDestroy() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            stopForeground(ID);
        }
        mWakeLock.release();
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        String ip = Utilities.getDeviceIP(this);
        int port = MainActivity.startServer(ip, 8090,
                Utilities.getExternalFile(MainActivity.DIRECTORY_STATIC).getAbsolutePath(),
                Utilities.getExternalFile(MainActivity.FILENAME_DATABASE).getAbsolutePath());

        Intent i = new Intent(this, MainActivity.class);
        i.setAction(MainActivity.ACTION_RUNNING);
        i.putExtra(MainActivity.EXTRA_URL, ip + ":" + port);
        this.sendBroadcast(intent);
        return super.onStartCommand(intent, flags, startId);
    }
}
