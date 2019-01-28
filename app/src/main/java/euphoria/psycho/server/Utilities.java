package euphoria.psycho.server;

import java.io.Closeable;
import java.io.DataOutput;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Reader;
import java.io.UTFDataFormatException;

public class Utilities {
    public static final long MB_IN_BYTES = 1048576;

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


}
