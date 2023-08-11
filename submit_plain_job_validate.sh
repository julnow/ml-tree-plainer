#/bin/bash

FILELIST="/lustre/cbm/users/tfic/ml-tree-plainer/filelist_test.txt"
OUTPUT_DIR="/lustre/cbm/users/tfic/pid_plain_trees/trees/validate"

LOG_DIR=$OUTPUT_DIR/log
mkdir -p $LOG_DIR
rm -rf $LOG_DIR/*
mkdir -p $LOG_DIR/out
mkdir -p $LOG_DIR/error

echo "logs can be found at $LOG_DIR"

FILENAME="plaintree12AGeV_DCM_1M.root"
sbatch --job-name="conv1M"\
        -t 6:00:00 \
        --partition main\
        --output=$LOG_DIR/out/%j.out.log \
        --error=$LOG_DIR/error/%j.err.log \
        -- $PWD/plain_job.sh $FILELIST $OUTPUT_DIR $FILENAME
