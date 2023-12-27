# poker-workshop
## Development environment
In order to ensure that all developers use the same environment, we use docker. This will avoid many problems with different versions of libraries and compilers. We will carry out the development using VSCode's Remote Development extension. This will allow us to do all our coding, compiling and testing inside a docker container.

### How to set up the development environment
If you are not using VSCode, you need to skip step 2 and instead of steps 5-8, you need to research how to attach to a docker container from your IDE.

1. Install [docker](https://docs.docker.com/get-docker/) and [git](https://github.com/git-guides/install-git) 
1. Install VSCode's Remote Development extension pack
1. Clone this repository to your local machine:
    ```bash
    cd <your workspace folder>
    git clone https://github.com/Duzzuti/poker-workshop.git
    ```
1. Set up the docker container:
    ```bash
    cd <your workspace folder>/poker-workshop
    docker build -t poker-workshop .
    docker run --name poker-workshop -h poker-workshop-container -v .:/home/dev/poker-workshop -it poker-workshop
    ```
1. Open the poker-workshop folder with VSCode
1. Open the `Remote Explorer` tab in VSCode
1. Right click on the `poker-workshop` container and click `Attach to container`
1. Install the required extensions in the container (C/C++, CMake)

