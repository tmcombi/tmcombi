# About the project
The project (http://www.tmcombi.org) is aimed at providing a production code for the trainable monotone combiner as well as first tests with real data. TMCombi is a free/open-source library running under MIT license.

# Useful links
Project home page:http://www.tmcombi.org

White paper at <cite>Neurocomputing</cite>: https://doi.org/10.1016/j.neucom.2020.07.075

White paper, free version: http://www.tmcombi.org/wp-content/uploads/2020/06/tmc.pdf

Continuous integration and testing pipeline (Jenkins): https://www.tmcombi.org/jenkins/job/tmcombi/

Current unit tests status: https://www.tmcombi.org/jenkins/job/tmcombi/lastCompletedBuild/testReport/


# Howto setup local build environment
First of all clone the repository:
```shell script
git clone https://github.com/tmcombi/tmcombi.git
```
The testing is designed to also test the build environment by creating a container with that environment as part of the pipeline. Thus you can just copy the instructions from envCMake/Dockerfile. At the time of writing the instructions are (for Linux/Ubuntu):
```shell script
apt-get update && apt-get install -y g++ ccache cmake libboost-all-dev git
git clone https://github.com/nushoin/RTree.git
```
# Howto build
We use cmake, so you can just use your favorite IDE and import the Project. You can of course build it from the command line. Build commandos are immer up to date as a part of the CI pipeline within the file Jenkins/tmcombi.Jenkinsfile. At the moment of writing this, these commands are:
```shell script
mkdir bin<br>
cd bin
cmake ../<br>
cmake --build .
```
# Howto run unit tests
CMake build multiple targets, at the moment of writing representing different unit tests. We use boost unit tests, so the parameter for the executables can be chosen accordingly. You can also consult Jenkins/tmcombi.Jenkinsfile, where we run all these unit tests.

# Next steps
Training kernel using boost graph library / linear optimization / e.t.c. (my own next step)

Visualization of point clouds / graph / layers (point subsets ) / borders. Suggested languges: c++ or python. Input: json files.

Evaluation (either writing from scratch or using some existing software). Suggested languages: c++ or python.
