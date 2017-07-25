#!/bin/sh
# This Script must be executed in Core 0
# Argument is a CPU core you executed test program (ex.com_fifo.exe)

THIS_PID=$$
TASKSET="taskset 0x01"

if [ $# -ge 3 ]
then 
	echo "Too many arguments"
	kill $THIS_PID
elif [ $# -eq 2 ]
then
	echo "Set core ID"
	TASKSET="taskset -c $2"
fi

# fifo connection test(GPOS to RTOS)
echo "Phase 1"
$TASKSET ./com_fifo_gtr.exe 16

echo "com fifo test"
$TASKSET ./com_fifo_gtr.exe 1 &
APP1_PID=$!
sleep 4
$TASKSET ./com_fifo_gtr.exe 6 &
APP2_PID=$!
sleep 4
echo "KILL process (pid = $APP1_PID)"
kill $APP1_PID
sleep 4
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
wait $APP2_PID

echo "Check other exit pattern"
$TASKSET ./com_fifo_gtr.exe 1 &
APP1_PID=$!
sleep 4
$TASKSET ./com_fifo_gtr.exe 6 &
APP2_PID=$!
sleep 4
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
wait $APP2_PID
sleep 4
echo "KILL process (pid = $APP1_PID)"
kill $APP1_PID
wait $APP1_PID

# fifo connection test(RTOS to GPOS)
echo "Phase 2"
$TASKSET ./com_fifo_rtg.exe 1 &
APP1_PID=$!
sleep 4
$TASKSET ./com_fifo_rtg.exe 6 &
APP2_PID=$!
sleep 4
echo "KILL process (pid = $APP1_PID)"
kill $APP1_PID
wait $APP1_PID
sleep 4
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
wait $APP2_PID

# fifo reconnect test
echo "Phase 3"
$TASKSET ./com_fifo_gtr.exe 6 &
APP2_PID=$!
sleep 2
$TASKSET ./com_fifo_reconnect.exe 1 0 & # do nothing
APP1_PID=$!
wait $APP1_PID
sleep 2
$TASKSET ./com_fifo_reconnect.exe 1 1 & # read message
APP1_PID=$!
wait $APP1_PID
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
wait $APP2_PID
$TASKSET ./com_fifo_reconnect.exe 1 0 & # do nothing
APP1_PID=$!
wait $APP1_PID
$TASKSET ./com_fifo_reconnect.exe 1 1 & # read message
APP1_PID=$!
wait $APP1_PID

# shmem test(RTOS to GPOS)
echo "Phase 4"
$TASKSET ./com_shmem_rtg.exe 0 0 &
APP1_PID=$!
sleep 4
$TASKSET ./com_shmem_rtg.exe 1 0 &
APP2_PID=$!
sleep 4
echo "KILL process (pid = $APP1_PID)"
kill $APP1_PID
sleep 4
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
wait $APP2_PID
sleep 4

# shmem test(GPOS to RTOS)
echo "Phase 5"
$TASKSET ./com_shmem_gtr.exe 0 0 &
APP1_PID=$!
sleep 4
$TASKSET ./com_shmem_gtr.exe 1 0 &
APP2_PID=$!
sleep 4
echo "KILL process (pid = $APP1_PID)"
kill $APP1_PID
sleep 4
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID

# fifo init test
echo "Phase 6"
$TASKSET ./com_fifo_gtr.exe 6 &
APP2_PID=$!
sleep 2
$TASKSET ./com_fifo_init.exe 1 0 &
APP1_PID=$!
wait $APP1_PID
sleep 2
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
$TASKSET ./com_fifo_init.exe 1 0 &
APP1_PID=$!
wait $APP1_PID

$TASKSET ./com_fifo_gtr.exe 6 &
APP2_PID=$!
sleep 2
$TASKSET ./com_fifo_init.exe 1 1 &
APP1_PID=$!
wait $APP1_PID
sleep 2
echo "KILL process (pid = $APP2_PID)"
kill $APP2_PID
$TASKSET ./com_fifo_init.exe 1 1 &
APP1_PID=$!
wait $APP1_PID
