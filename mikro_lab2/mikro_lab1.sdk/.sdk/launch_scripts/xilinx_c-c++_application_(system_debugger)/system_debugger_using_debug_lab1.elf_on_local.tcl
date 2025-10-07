connect -url tcp:127.0.0.1:3121
source C:/Designs/komo/mikro_lab1/mikro_lab1.sdk/microprocessor_block_wrapper_hw_platform_0/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351A82522A"} -index 0
loadhw -hw C:/Designs/komo/mikro_lab1/mikro_lab1.sdk/microprocessor_block_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351A82522A"} -index 0
stop
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351A82522A"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351A82522A"} -index 0
dow C:/Designs/komo/mikro_lab1/mikro_lab1.sdk/lab1/Debug/lab1.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351A82522A"} -index 0
con
