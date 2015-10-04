package com.fleetcents.generic_tpms.core;

import com.fleetcents.generic_tpms.R;
import com.fleetcents.generic_tpms.core.RtlSdrStartException;
import com.fleetcents.generic_tpms.core.RtlSdrStartException.err_info;
import com.fleetcents.generic_tpms.core.StrRes;

public class RtlSdrException extends Exception {

	private static final long serialVersionUID = 9112234577039075951L;
	public final int id;

	static final String translateToString(final int id) {
		switch (id) {
		case RtlSdr.LIBUSB_ERROR_IO:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_IO");
		case RtlSdr.LIBUSB_ERROR_INVALID_PARAM:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_INVALID_PARAM");
		case RtlSdr.LIBUSB_ERROR_ACCESS:
			return StrRes.get(R.string.exception_LIBUSB_ERROR_ACCESS);
		case RtlSdr.LIBUSB_ERROR_NO_DEVICE:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_NO_DEVICE");
		case RtlSdr.LIBUSB_ERROR_NOT_FOUND:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_NOT_FOUND");
		case RtlSdr.LIBUSB_ERROR_BUSY:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_BUSY");
		case RtlSdr.LIBUSB_ERROR_TIMEOUT:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_TIMEOUT");
		case RtlSdr.LIBUSB_ERROR_OVERFLOW:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_OVERFLOW");
		case RtlSdr.LIBUSB_ERROR_PIPE:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_PIPE");
		case RtlSdr.LIBUSB_ERROR_INTERRUPTED:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_INTERRUPTED");
		case RtlSdr.LIBUSB_ERROR_NO_MEM:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_NO_MEM");
		case RtlSdr.LIBUSB_ERROR_NOT_SUPPORTED:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_NOT_SUPPORTED");
		case RtlSdr.LIBUSB_ERROR_OTHER:
			return StrRes.get(R.string.exception_LIBUSB_GENERIC, "LIBUSB_ERROR_OTHER");

		case RtlSdr.EXIT_OK:
			return StrRes.get(R.string.exception_OK);
		case RtlSdr.EXIT_WRONG_ARGS:
			return StrRes.get(R.string.exception_WRONG_ARGS);
		case RtlSdr.EXIT_INVALID_FD:
			return StrRes.get(R.string.exception_INVALID_FD);
		case RtlSdr.EXIT_NO_DEVICES:
			return StrRes.get(R.string.exception_NO_DEVICES);
		case RtlSdr.EXIT_FAILED_TO_OPEN_DEVICE:
			return StrRes.get(R.string.exception_FAILED_TO_OPEN_DEVICE);
		case RtlSdr.EXIT_CANNOT_RESTART:
			return StrRes.get(R.string.exception_CANNOT_RESTART);
		case RtlSdr.EXIT_CANNOT_CLOSE:
			return StrRes.get(R.string.exception_CANNOT_CLOSE);
		case RtlSdr.EXIT_UNKNOWN:
			return StrRes.get(R.string.exception_UNKNOWN);
		case RtlSdr.EXIT_SIGNAL_CAUGHT:
			return StrRes.get(R.string.exception_SIGNAL_CAUGHT);
    case RtlSdr.EXIT_NOT_ENOUGH_POWER:
      return StrRes.get(R.string.exception_NOT_ENOUGH_POWER);
    case RtlSdr.RTLSDR_FILENAME_NOT_SPECIFIED:
      return StrRes.get(R.string.exception_RTLSDR_FILENAME_NOT_SPECIFIED);

		default:
			return StrRes.get(R.string.exception_DEFAULT, id);
		}
	}

	public final RtlSdrStartException.err_info getReason() {
		switch (id) {
		case RtlSdr.LIBUSB_ERROR_IO:
			return err_info.no_devices_found;
		case RtlSdr.LIBUSB_ERROR_INVALID_PARAM:
			return err_info.unknown_error;
		case RtlSdr.LIBUSB_ERROR_ACCESS:
			return err_info.permission_denied;
		case RtlSdr.LIBUSB_ERROR_NO_DEVICE:
			return err_info.no_devices_found;
		case RtlSdr.LIBUSB_ERROR_NOT_FOUND:
			return err_info.no_devices_found;
		case RtlSdr.LIBUSB_ERROR_BUSY:
			return err_info.already_running;
		case RtlSdr.LIBUSB_ERROR_TIMEOUT:
			return err_info.no_devices_found;
		case RtlSdr.LIBUSB_ERROR_OVERFLOW:
			return err_info.no_devices_found;
		case RtlSdr.LIBUSB_ERROR_PIPE:
			return err_info.unknown_error;
		case RtlSdr.LIBUSB_ERROR_INTERRUPTED:
			return err_info.unknown_error;
		case RtlSdr.LIBUSB_ERROR_NO_MEM:
			return err_info.unknown_error;
		case RtlSdr.LIBUSB_ERROR_NOT_SUPPORTED:
			return err_info.no_devices_found;
		case RtlSdr.LIBUSB_ERROR_OTHER:
			return err_info.unknown_error;

		case RtlSdr.EXIT_OK:
			return null;
		case RtlSdr.EXIT_WRONG_ARGS:
			return err_info.unknown_error;
		case RtlSdr.EXIT_INVALID_FD:
			return err_info.permission_denied;
		case RtlSdr.EXIT_NO_DEVICES:
			return err_info.no_devices_found;
		case RtlSdr.EXIT_FAILED_TO_OPEN_DEVICE:
			return err_info.no_devices_found;
		case RtlSdr.EXIT_CANNOT_RESTART:
			return err_info.unknown_error;
		case RtlSdr.EXIT_CANNOT_CLOSE:
			return err_info.replug;
		case RtlSdr.EXIT_UNKNOWN:
			return err_info.unknown_error;
		case RtlSdr.EXIT_SIGNAL_CAUGHT:
			return err_info.unknown_error;
    case RtlSdr.EXIT_NOT_ENOUGH_POWER:
      return err_info.unknown_error;
    case RtlSdr.RTLSDR_FILENAME_NOT_SPECIFIED:
      return err_info.unknown_error;
    case RtlSdr.RTLSDR_FILE_NOT_SAVED:
      return err_info.unknown_error;

		default:
			return null;
		}
	}

	public RtlSdrException(final int id) {
		super(translateToString(id));
		this.id = id;
	}

}
