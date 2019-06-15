# THIS PROJECT IS UNMAINTAINED AND IS HERE FOR ARCHIVAL PURPOSES.
# WHEN BUILT, THE DLL WORKS AS IS FOR THE CABALMAIN THAT IS USED BY MOST PRIVATE SERVERS, BUT NO FURTHER IMPROVEMENTS WILL BE MADE.
# PULL REQUESTS WILL NOT BE ACCEPTED.

To use this DLL, build in `Release` mode, then load the DLL (crash_reporter.bin) inside your cabalmain (via either `LoadLibrary` or another DLL) and call the `blue` function.

NOTE: `blue` must be called ***before*** any anticheat loads (MShield, XTrap, etc.)
