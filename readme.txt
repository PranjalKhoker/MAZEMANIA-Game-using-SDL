IITD GAME - MAZEMANIA (Developer - Pranjal Khoker)
<<<<<<< HEAD



Installion of necessary Libraries to run the game- (IGNORE IF YOU HAVE SDL ALREADY INSTALLED ON YOUR SYSTEM) 

For Ubuntu users- open terminal and type the following commands

1. apt-cache search libsdl2
2. apt-get install libsdl2-dev
3. apt-cache search libsdl2-image
4. apt-get install libsdl2-image-dev
5. apt-get install libsdl2-ttf-dev
6. apt-get install libsdl2-mixer-dev


How to run the game ?

open terminal and type the following commands

1. g++ game_client.cpp  -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o client
2. g++ game_server.cpp  -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o server

To run on one system

3. open another terminal and run ./server
4. in first terminal, run ./client

To run on two different Systems

3. connect both system over a wi-fi
4. set the ip address of both server and client accordingly
5. in one system, run ./server
6. in other, run ./client

Resources from where i learnt about SDL 
1. lets make games youtube channel, link = https://www.youtube.com/c/CarlBirch
2. lazyfoo website, link = https://lazyfoo.net/tutorials/SDL/index.php



=======



Installion of necessary Libraries to run the game- (IGNORE IF YOU HAVE SDL ALREADY INSTALLED ON YOUR SYSTEM) 

For Ubuntu users- open terminal and type the following commands

1. apt-cache search libsdl2
2. apt-get install libsdl2-dev
3. apt-cache search libsdl2-image
4. apt-get install libsdl2-image-dev
5. apt-get install libsdl2-ttf-dev
6. apt-get install libsdl2-mixer-dev


How to run the game ?

open terminal and type the following commands

1. g++ game_client.cpp  -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o client
2. g++ game_server.cpp  -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o server

To run on one system

3. open another terminal and run ./server
4. in first terminal, run ./client

To run on two different Systems

3. connect both system over a wi-fi
4. set the ip address of both server and client accordingly
5. in one system, run ./server
6. in other, run ./client

Resources from where i learnt about SDL 
1. lets make games youtube channel, link = https://www.youtube.com/c/CarlBirch
2. lazyfoo website, link = https://lazyfoo.net/tutorials/SDL/index.php


>>>>>>> 7555677090d8b7f9503d41d913be2e25debfac88



