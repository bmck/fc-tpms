package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

public class RtlSdrException extends Exception {

    private static final long serialVersionUID = 9112234577039075951L;
    public final int id;

    public static final String translateToString(final int id) {
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
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.LIBUSB_ERROR_INVALID_PARAM:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.LIBUSB_ERROR_ACCESS:
                return RtlSdrStartException.err_info.permission_denied;
            case RtlSdr.LIBUSB_ERROR_NO_DEVICE:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.LIBUSB_ERROR_NOT_FOUND:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.LIBUSB_ERROR_BUSY:
                return RtlSdrStartException.err_info.already_running;
            case RtlSdr.LIBUSB_ERROR_TIMEOUT:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.LIBUSB_ERROR_OVERFLOW:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.LIBUSB_ERROR_PIPE:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.LIBUSB_ERROR_INTERRUPTED:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.LIBUSB_ERROR_NO_MEM:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.LIBUSB_ERROR_NOT_SUPPORTED:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.LIBUSB_ERROR_OTHER:
                return RtlSdrStartException.err_info.unknown_error;

            case RtlSdr.EXIT_OK:
                return null;
            case RtlSdr.EXIT_WRONG_ARGS:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.EXIT_INVALID_FD:
                return RtlSdrStartException.err_info.permission_denied;
            case RtlSdr.EXIT_NO_DEVICES:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.EXIT_FAILED_TO_OPEN_DEVICE:
                return RtlSdrStartException.err_info.no_devices_found;
            case RtlSdr.EXIT_CANNOT_RESTART:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.EXIT_CANNOT_CLOSE:
                return RtlSdrStartException.err_info.replug;
            case RtlSdr.EXIT_UNKNOWN:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.EXIT_SIGNAL_CAUGHT:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.EXIT_NOT_ENOUGH_POWER:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.RTLSDR_FILENAME_NOT_SPECIFIED:
                return RtlSdrStartException.err_info.unknown_error;
            case RtlSdr.RTLSDR_FILE_NOT_SAVED:
                return RtlSdrStartException.err_info.unknown_error;

            default:
                return null;
        }
    }

    public RtlSdrException(final int id) {
        super(translateToString(id));
        this.id = id;
    }

}
