# ml_tree_plainer

## General information

at_tree_plainer is developed for conversion of complex AnalysisTree structures into the plain tree format (either AnalysisTree or ROOT::TTree).

It is based on at_tree_plainer by Oleksii Lubynets.


## Pre-requirements

### Root

ROOT6 is needed for installation:

https://root.cern/install/build_from_source/

Follow instructions
    
### AnalysisTree

https://github.com/HeavyIonAnalysis/AnalysisTree

Follow instructions

### Boost

https://www.boost.org/

As of 25.08.2023 boost 1.78.0 bundled with FairSoft is used.

## Installation

Clone at_tree_plainer

    git clone git@github.com:julnow/ml-tree-plainer.git
    
Source ROOT

    source <path-to-root>/install/bin/thisroot.sh
    
Export AnalysisTree libraries

    export AnalysisTree_DIR=<path-to-analysistree>/install/lib/cmake/AnalysisTree
    
Export Boost dir

    export BOOST_DIR=<path-to-fairsoft-install>/lib/cmake/Boost-1.78.0

Install at_tree_plainer
    
    mkdir build install
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../install ..
    make -j install
For debugging the flag
    ``DCMAKE_BUILD_TYPE=DEBUG``
should be added
    
## Run locally

./main config.json

example config.json:

```
{
    "branches": {
        "TofHits": {
            "vars": [
                {
                    "inName": "mass2",
                    "outName": "mass2",
                    "type": "float"
                }
            ]
        }
    },
    "inputFileList": "~/filelist.txt",
    "outputFileName": "~/test.root",
    "outputBranchName": "Complex"
}
```

Where `filelist.txt` contains paths to PFSimple output AnalysisTree files. 
If one wishes to add more fields or more branches, they just need to be added to the json config.


As of now, the available branches are:
- TofHits
- SimParticles
- EventHeader
- VtxTracks
- TrdTracks

And they can be easily expanded in the future.

## Run with Slurm

To use with slurm, go into the `scripts` directory and :

```
./submit_job <path-to-cofig-file> <path-to-output-dir>
```


Tested only on GSI Green Cube slurm setup.
