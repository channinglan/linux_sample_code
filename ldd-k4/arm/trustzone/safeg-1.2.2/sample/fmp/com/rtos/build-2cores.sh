make realclean &&
make TARGET=vexpressa9_gcc SAFEG=SECURE ENABLE_QEMU=true PRC_NUM=2 ENABLE_G_SYSLOG=true fmp.bin &&
sudo cp fmp.bin /var/lib/tftpboot/t.bin &&
make realclean

