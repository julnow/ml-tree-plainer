#!/bin/bash

source /cvmfs/fairsoft.gsi.de/debian10/fairsoft/apr22/bin/thisroot.sh
root --version
LD_LIBRARY_PATH="/lustre/cbm/users/tfic/AnalysisTree/install/lib:$LD_LIBRARY_PATH"
LD_LIBRARY_PATH="/lustre/cbm/users/tfic/at_tree_plainer/install/lib:$LD_LIBRARY_PATH"
echo "$LD_LIBRARY_PATH"

CONFIG="/lustre/cbm/users/tfic/at_tree_plainer/example_config.json"

/lustre/cbm/users/tfic/at_tree_plainer/install/bin/main $CONFIG

