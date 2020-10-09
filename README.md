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

#git clone https://github.com/tmcombi/tmcombi.git

The testing is designed to also test the build environment by creating a container with that environment as part of the pipeline. Thus you can just copy the instructions from envCMake/Dockerfile. At the time of writing the instructions are (for Linux/Ubuntu):

#apt-get update && apt-get install -y g++ ccache cmake libboost-all-dev git<br>
#git clone https://github.com/nushoin/RTree.git

# Howto build
We use cmake, so you can just use your favorit IDE and import the Project. You can of course build it from the command line. Build commandos are immer up to date as a part of the CI pipeline within the file Jenkins/tmcombi.Jenkinsfile. At the moment of writing this, these commandos are:

#mkdir bin<br>
#cmake ../<br>
#cmake --build .



# Howto run unit tests


# Next steps
