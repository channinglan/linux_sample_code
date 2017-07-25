#!/bin/bash

debugfs=/sys/kernel/debug
echo nop > $debugfs/tracing/current_tracer
echo 0 > $debugfs/tracing/tracing_on
echo $$ > $debugfs/tracing/set_ftrace_pid
echo function_graph > $debugfs/tracing/current_tracer
#replace vfs_write by your function name
echo vfs_write > $debugfs/tracing/set_graph_function
echo 1 > $debugfs/tracing/tracing_on
exec "$@" 

