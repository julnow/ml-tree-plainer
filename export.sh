#!/bin/bash

#ROOT
source /cvmfs/fairsoft.gsi.de/debian10/fairsoft/apr22/bin/thisroot.sh
TFIC_DIR=/lustre/cbm/users/tfic

#AnalysisTree
source $TFIC_DIR/AnalysisTree/install/bin/AnalysisTreeConfig.sh
export AnalysisTree_DIR=$TFIC_DIR/AnalysisTree/install

#OnnxRuntime
export OnnxRuntime_DIR=$TFIC_DIR/onnxruntime/install

#Boost
export BOOST_DIR=/cvmfs/fairsoft.gsi.de/debian10/fairsoft/apr22/lib/cmake/Boost-1.78.0