#/bin/bash

FILELIST="/lustre/cbm/users/tfic/ml-tree-plainer/filelist_train.txt"
OUTPUT_DIR="/lustre/cbm/users/tfic/pid_plain_trees/trees/train"

LOG_DIR=$OUTPUT_DIR/log
mkdir -p $LOG_DIR
rm -rf $LOG_DIR/*
mkdir -p $LOG_DIR/out
mkdir -p $LOG_DIR/error

echo "logs can be found at $LOG_DIR"

FILENAME="plaintree12AGeV_DCM_4M.root"
sbatch --job-name="conv4M"\
        -t 8:00:00 \
        --partition main\
        --output=$LOG_DIR/out/%j.out.log \
        --error=$LOG_DIR/error/%j.err.log \
        -- $PWD/plain_job.sh $FILELIST $OUTPUT_DIR $FILENAME
