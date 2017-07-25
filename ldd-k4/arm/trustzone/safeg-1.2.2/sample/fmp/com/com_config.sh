#export DOSCOM_CONFIG=$(pwd)/config.txt
cp ./config.txt ../../../com/configurator/
cd ../../../com/configurator/
make
cd -
../../../com/configurator/configurator.exe -fmp
cd ../../../com/configurator/
make clean
cd -
