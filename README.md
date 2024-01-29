# poker-workshop
This is a playground for poker.
The goal is to create a poker game that can be played by multiple computer players.

There are multiple objectives for this project:
- simulate a poker game
- create a poker file format to save games and states
- create poker AIs with different strategies and play them against each other
- create an environment to train poker AIs
- create an API to allow anybody to create their own poker AIs

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

## Docs

### How to generate the docs
If you followed the docker setup, you can generate the docs easily:
1. You have to be in the container shell
1. You have to be in the poker-workshop folder

```sh
bash gen_doxy.sh
```

or generate them manually:

```sh
doxygen doxygen_config
```

Now you should have a `html` folder in the working directory. Open the `html/index.html` file in your browser to view the docs.

### Docs structure

We are using a [Game class](docs/game.md) to simulate one set of poker games. The game class is responsible for the game loop and the game logic. Every time a [Player](docs/player.md) has to make a turn, it calls the player´s `turn` method with the [Data](docs/data.md) struct, which holds all information about the game. The game class simulates a [Deck](docs/deck.md) of cards. Who wins the showdown is determined by the [Handstrength](docs/handstrengths.md) of the players.

There are several implemented [Players](docs/players.md) with different strategies.