""" Utilities for handling exceptions """
import sys
import traceback
import functools


printerr = functools.partial(print, file=sys.stderr)


def exc_str(full_tb=False):
    return _exc_str(*sys.exc_info(), full_tb)

def _exc_str(exc_type, exc_value, exc_tb, full_tb=False):
    result = ""
    if _exc_exists(exc_type, exc_value, exc_tb):
        all_info = [exc_type.__name__, "\n", str(exc_value), "\n"]
        if full_tb:
            all_info.extend(traceback.format_tb(exc_tb))
            result = "".join(all_info)
    return result


def _exc_exists(exc_type, exc_value, exc_tb):
    return all([exc_type, exc_value, exc_tb])


class TracebackContext:
    def __init__(self, full_tb=False):
        self.full_tb = full_tb


    def __enter__(self):
        return self


    def __exit__(self, exc_type, exc_value, exc_tb):
        if _exc_exists(exc_type, exc_value, exc_tb):
            exc = _exc_str(exc_type, exc_value, exc_tb, self.full_tb)
            printerr(exc)
        return True
