LSMonitor: camonitor.c pgaccess.c tool_lib.c tool_lib.h pgaccess.h Makefile
	gcc -I /contrabass/epics/base/include -I /contrabass/epics/base/include/os/Linux -o LSMonitor camonitor.c pgaccess.c tool_lib.c -L /contrabass/epics/base/lib/linux-x86 -lca -lpq -lm


