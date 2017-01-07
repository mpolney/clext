# clext: Extractor for Clan Lord CL_Images file

This tool extracts all the image resources from the data files
for an old game called [Clan Lord][1]. I wrote this code some time
in 2008 and figured out the format by inspecting the binaries of
a program capable of reading the format using GDB on OS X. The
doc/ directory contains the documentation of the format. A
CL_Images file can be obtained from [this][2] ZIP archive. Note
that it's possible that in a future version the developer may
change the format. However, it has been tested with several
versions over the years since I wrote it and it has always worked.

[1]: https://www.deltatao.com/clanlord/
[2]: https://www.deltatao.com/downloads/clanlord/win32/ClanLord.zip
