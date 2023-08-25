#!/bin/bash

CONFIG_PATH=$1
OUTPUT_DIR=$2
mkdir -p $OUTPUT_DIR

#Change the following paths if need be:
source /cvmfs/fairsoft.gsi.de/debian10/fairsoft/apr22/bin/thisroot.sh
ANALYSIS_TREE_INSTALL_DIR="/lustre/cbm/users/tfic/AnalysisTree/install"
AT_TREE_PLAINER_INSTALL_DIR="/lustre/cbm/users/tfic/at_tree_plainer/install"

LD_LIBRARY_PATH="$ANALYSIS_TREE_INSTALL_DIR/lib:$LD_LIBRARY_PATH"
LD_LIBRARY_PATH="$AT_TREE_PLAINER_INSTALL_DIR/lib:$LD_LIBRARY_PATH"

echo "$LD_LIBRARY_PATH"
cd $OUTPUT_DIR

EXE=$AT_TREE_PLAINER_INSTALL_DIR/bin/at_tree_plainer

$EXE $CONFIG_PATH

