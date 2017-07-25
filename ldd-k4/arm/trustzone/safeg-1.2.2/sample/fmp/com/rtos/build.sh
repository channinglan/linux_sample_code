make realclean &&
make TARGET=vexpressa9_gcc SAFEG=SECURE ENABLE_QEMU=true PRC_NUM=1 ENABLE_G_SYSLOG=false fmp.bin &&
sudo cp fmp.bin /var/lib/tftpboot/t.bin
