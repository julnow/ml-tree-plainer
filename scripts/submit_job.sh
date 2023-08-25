#!/bin/bash

CONFIG_PATH=$1
OUTPUT_DIR=$2

CONFIG_PATH=$PWD/configs/tof-trd-primaries.json
OUTPUT_DIR=/lustre/cbm/users/tfic/pid_plain_trees/trees/test



LOG_DIR=$OUTPUT_DIR/log
mkdir -p $LOG_DIR
rm -rf $LOG_DIR/*
mkdir -p $LOG_DIR/out
mkdir -p $LOG_DIR/error

echo "logs can be found at $LOG_DIR"

sbatch --job-name="at_plain"\
        -t 8:00:00 \
        --partition main\
        --output=$LOG_DIR/out/%j.out.log \
        --error=$LOG_DIR/error/%j.err.log \
        -- $PWD/plain_job.sh $CONFIG_PATH $OUTPUT_DIR
