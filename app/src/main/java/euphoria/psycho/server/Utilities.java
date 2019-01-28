package euphoria.psycho.server;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Environment;
import android.util.Log;

import java.io.Closeable;
import java.io.DataOutput;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Reader;
import java.io.UTFDataFormatException;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class Utilities {
    public static final String EXTRA_URL = "url";
    public static final long MB_IN_BYTES = 1048576;
    private static final String TAG = "TAG/" + Utilities.class.getSimpleName();

    public static void closeQuietly(final Closeable closeable) {
        try {
            if (closeable != null) {
                closeable.close();
            }
        } catch (final IOException ioe) {
            // ignore
        }
    }

    public static void copy(InputStream inputStream, long maxSize, OutputStream outputStream) throws IOException {
        final byte[] buffer = new byte[4096];
        long toRead = maxSize;
        while (toRead > 0) {
            int read = inputStream.read(buffer, 0, (int) Math.min(buffer.length, toRead));
            if (read < 0) break;
            toRead -= read;
            outputStream.write(buffer, 0, read);
        }
    }

    public static void createDirectoryIfNotExist(File dir) {
        if (!dir.isDirectory()) dir.mkdirs();
    }

    public static NotificationChannel createNotificationChannel(Context context, String id, CharSequence name) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {

            NotificationChannel channel = new NotificationChannel(
                    id,
                    name,
                    NotificationManager.IMPORTANCE_LOW);
            ((NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE))
                    .createNotificationChannel(channel);
            return channel;
        }
        return null;
    }

    public static String getDeviceIP(Context context) {
        WifiManager wifiManager = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        try {
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();


            InetAddress inetAddress = intToInetAddress(wifiInfo.getIpAddress());

            return inetAddress.getHostAddress();
        } catch (Exception e) {
            Log.e(TAG, "[getDeviceIP] ---> ", e);
            return null;
        }
    }

    public static File getExternalFile(String fileName) {
        return new File(Environment.getExternalStorageDirectory(), fileName);
    }

    public static InetAddress intToInetAddress(int hostAddress) {
        byte[] addressBytes = {(byte) (0xff & hostAddress),
                (byte) (0xff & (hostAddress >> 8)),
                (byte) (0xff & (hostAddress >> 16)),
                (byte) (0xff & (hostAddress >> 24))};

        try {
            return InetAddress.getByAddress(addressBytes);
        } catch (UnknownHostException e) {
            throw new AssertionError();
        }
    }
}
