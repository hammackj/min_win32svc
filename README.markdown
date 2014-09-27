# min_win32svc

This is a simple Windows Service. It is based on several Windows Service samples I saw on the net and MSDN. None of which were really simple.

# Installation

	C:\>sc create "Min Windows Service" binPath= C:\min_win32svc.exe

# Uninstallation

	C:\>sc delete "Min Windows Service"


# Notes
The coding style conforms to the Linux Kernel standards as much I as I could do. (I ran checkpatch.pl). The Makefile also builds the service under mingw.

# Issues

