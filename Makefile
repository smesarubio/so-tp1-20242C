all: slave app view

slave: slave.c
	gcc -Wall slave.c -o slave -std=c99 -lm -lrt -pthread -g -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L

app: app.c
	gcc -Wall app.c -o md5 -std=c99 -lm -lrt -pthread -g -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L

view: view.c
	gcc -Wall view.c -o view -std=c99 -lm -lrt -pthread -g -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L

clean:
	rm -f slave app view result.txt PVS-Studio.log report.tasks strace_out
	rm -rf .config

pvs-studio:
	pvs-studio-analyzer credentials PVS-Studio Free FREE-FREE-FREE-FREE
	pvs-studio-analyzer trace -- make -j8
	pvs-studio-analyzer analyze -j2 -l /root/.config/PVS-Studio/PVS-Studio.lic -o PVS-Studio.log
	plog-converter -a GA:1,2 -t tasklist -o report.tasks PVS-Studio.log

.PHONY: all clean
