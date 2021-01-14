# About the project
The project (http://www.tmcombi.org) is aimed at providing a production code for the trainable monotone combiner as well as first tests with real data. TMCombi is a free/open-source library running under MIT license.

Currently only Linux/Ubuntu is supported. If you are interested in other OS, do not hesitate to contact me.

# Useful links
Project home page:http://www.tmcombi.org

White paper at <cite>Neurocomputing</cite>: https://doi.org/10.1016/j.neucom.2020.07.075

White paper, free version: http://www.tmcombi.org/wp-content/uploads/2020/06/tmc.pdf

Continuous integration and testing pipeline (Jenkins): https://www.tmcombi.org/jenkins/job/tmcombi/

Current unit tests status: https://www.tmcombi.org/jenkins/job/tmcombi/lastCompletedBuild/testReport/

# How to try the training and evaluation
Step 1. Get the dependencies (ubuntu) and clone the repository:
```shell script
apt-get update && apt-get install -y g++ ccache cmake libboost-all-dev git
git clone https://github.com/Sergey-Grosman/DynDimRTree.git
git clone https://github.com/nushoin/RTree.git
git clone https://github.com/tmcombi/tmcombi.git
```
Remark: if the instructions to build up a build environment above are not any more up-to-date, then copy current instructions from envCMake/Dockerfile, which is a part of the testing pipeline and is beeing kept up-to-date.

Step 2. Go into the directory and build the main and one small example:
```shell script
cd tmcombi
mkdir bin
cd bin
cmake ../
cmake --build . --target tmc
cmake --build . --target tmc_classify_example
cd ..
```

Step 3. Perform a training with a 2D test sample of size 992:
```shell script
bin/tmc --names data/tmc_paper/tmc_paper.names \
        --train-data data/tmc_paper/tmc_paper.data \
        --eval-data data/tmc_paper/tmc_paper.test \
        --trained-config tmc_paper_border_system.json
```
You should get output like this:
```shell script
Names file was set to data/tmc_paper/tmc_paper.names
Train data file was set to data/tmc_paper/tmc_paper.data
Evaluation data file was set to data/tmc_paper/tmc_paper.test
Training sample loaded: 992 unique feature vectors
Evaluation sample loaded: 992 unique feature vectors
Starting tmc optimization... finished in 28 iterations
Resulting layer partitioning size: 29
########################## Performing evaluation ######################################
		Train conf matr	###	Eval conf matr
predicted pos:	415	65	###	396.5	80.5
predicted neg:	81	431	###	99.5	415.5
actually   ->	pos	neg	###	pos	neg
---------------------------------------------------------------------------------------
		Train rank err	###	Eval rank err
		0.0701824	###	0.110001
---------------------------------------------------------------------------------------
		Train err rate	###	Eval err rate
		0.147177	###	0.181452
#######################################################################################
Border system is dumped into file: tmc_paper_border_system.json
```

Step 4. Run a classification example with a configuration trained in the previous step:
 ```shell script
bin/tmc_classify_example --trained-config tmc_paper_border_system.json
```
which should produce an output like this:
 ```shell script
Trained configuration file was set to tmc_paper_border_system.json
Loading boder system from file: tmc_paper_border_system.json
Classifying two first vectors of positives and negatives from the evaluation data sample
Vector {0.996323,0.805821} from evaluation data sample has conf = 1 (compared to 0.5) and therefore is classified as "1"
Vector {0.452852,0.291811} from evaluation data sample has conf = 0.424242 (compared to 0.5) and therefore is classified as "0"
Vector {0.187739,0.031114} from evaluation data sample has conf = 0.0625 (compared to 0.5) and therefore is classified as "0"
Vector {0.046231,0.153799} from evaluation data sample has conf = 0.15942 (compared to 0.5) and therefore is classified as "0"
```
If you have issues in running the training or evaluation for your data, please contact the authors.

# Howto run unit tests
At the moment of writing CMake builds multiple targets representing different unit tests. We use boost unit tests. You can also consult Jenkins/tmcombi.Jenkinsfile, where we run all these unit tests.


# Next steps
Visualization of point clouds / layer partitioning / border system. Suggested languages: c++ or python. Input: json files.

Incorporate feather data sets.

Incorporate boosting.

Incorporate bagging:
1) split train sample into subsamples
2) train tmc for each subsample
3) train tmc merging results of tmcs from previous step
4) (optional) try deeper architectures generalizing previous 3 steps
