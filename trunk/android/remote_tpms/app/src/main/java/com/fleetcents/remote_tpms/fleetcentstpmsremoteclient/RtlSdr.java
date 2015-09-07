package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

import android.util.Log;

public class RtlSdr {
	private final static String TAG = "RTLSDR";

	public final static int LIBUSB_ERROR_IO = -1;
	public final static int LIBUSB_ERROR_INVALID_PARAM = -2;
	public final static int LIBUSB_ERROR_ACCESS = -3;
	public final static int LIBUSB_ERROR_NO_DEVICE = -4;
	public final static int LIBUSB_ERROR_NOT_FOUND = -5;
	public final static int LIBUSB_ERROR_BUSY = -6;
	public final static int LIBUSB_ERROR_TIMEOUT = -7;
	public final static int LIBUSB_ERROR_OVERFLOW = -8;
	public final static int LIBUSB_ERROR_PIPE = -9;
	public final static int LIBUSB_ERROR_INTERRUPTED = -10;
	public final static int LIBUSB_ERROR_NO_MEM = -11;
	public final static int LIBUSB_ERROR_NOT_SUPPORTED = -12;
	public final static int LIBUSB_ERROR_OTHER = -99;

	public final static int EXIT_OK = 0;
	public final static int EXIT_WRONG_ARGS = 1;
	public final static int EXIT_INVALID_FD = 2;
	public final static int EXIT_NO_DEVICES = 3;
	public final static int EXIT_FAILED_TO_OPEN_DEVICE = 4;
	public final static int EXIT_CANNOT_RESTART = 5;
	public final static int EXIT_CANNOT_CLOSE = 6;
	public final static int EXIT_UNKNOWN = 7;
	public final static int EXIT_SIGNAL_CAUGHT = 8;
	public final static int EXIT_NOT_ENOUGH_POWER = 9;
	public final static int RTLSDR_FILENAME_NOT_SPECIFIED = 10;
	public final static int RTLSDR_FILE_NOT_SAVED = 11;

	private final static Object locker = new Object();
	private final static Object exitcode_locker = new Object();
	private final static ArrayList<OnProcessTalkCallback> talk_callacks = new ArrayList<RtlSdr.OnProcessTalkCallback>();

	private static volatile AtomicInteger exitcode = new AtomicInteger(EXIT_UNKNOWN);
	private static volatile AtomicBoolean exitcode_set = new AtomicBoolean(false);

	public static native void open(final String args, final int fd, final String uspfs_path);// throws RtlSdrException;
	public static native void close();// throws RtlSdrException;
	public static native boolean isNativeRunning();

	private static void printf_receiver(final String data) {
		for (final OnProcessTalkCallback c : talk_callacks)
			c.OnProcessTalk(data);
		Log.d(TAG, data);
	}

	private static void printf_stderr_receiver(final String data) {
		for (final OnProcessTalkCallback c : talk_callacks)
			c.OnProcessTalk(data);
		Log.w(TAG, data);
	}

	private static void onclose(int exitcode) {
		RtlSdr.exitcode.set(exitcode);
		exitcode_set.set(true);
		synchronized (exitcode_locker) {
			exitcode_locker.notifyAll();
		}
		if (exitcode != EXIT_OK)
			Log.e(TAG, "Exitcode "+exitcode);
		else
			Log.d(TAG, "Exited with success");
	}

	private static void onopen() {
		for (final OnProcessTalkCallback c : talk_callacks)
			c.OnOpened();
		Log.d(TAG, "Device open");
	}

	public static void registerWordCallback(final OnProcessTalkCallback callback) {
		if (!talk_callacks.contains(callback)) talk_callacks.add(callback);
	}

	public static void unregisterWordCallback(final OnProcessTalkCallback callback) {
		talk_callacks.remove(callback);
	}


	public static void start(final String args, final int fd, final String uspfs_path) throws RtlSdrException, InterruptedException {
		if (isNativeRunning()) {
			close();
			try {
				synchronized (locker) {
					locker.wait(50);
				}
			} catch (InterruptedException e) {}

			if (isNativeRunning()) throw new RtlSdrException(EXIT_CANNOT_RESTART);
		}

		Thread t = new Thread() {
			public void run() {
				exitcode_set.set(false);
				exitcode.set(EXIT_UNKNOWN);

				open(args, fd, uspfs_path);

				if (!exitcode_set.get()) {
					close();
					try {
						synchronized (exitcode_locker) {
							exitcode_locker.wait(100);
						}
					} catch (InterruptedException e) {}
				}

				if (!exitcode_set.get())
					exitcode.set(EXIT_CANNOT_CLOSE);

				RtlSdrException e = null;
				final int exitcode = RtlSdr.exitcode.get();
				if (exitcode != EXIT_OK) e = new RtlSdrException(exitcode);

				for (final OnProcessTalkCallback c : talk_callacks)
					c.OnClosed(exitcode, e);

				synchronized (locker) {
					locker.notifyAll();
				}
			}
		};
		t.start();
		t.join();
	}

	public static void stop() {
		if (!isNativeRunning()) return;
		close();
	}

	public interface OnProcessTalkCallback {
		/** Whenever the process writes something to its stdout, this will get called */
		void OnProcessTalk(final String line);

		void OnClosed(final int exitvalue, final RtlSdrException e);

		void OnOpened();
	}

}
