# C-Snake-NN
## WIP Readme
snake game using SDL2 for graphics and having a simple AI mode and a Neural Network, and a manual mode that has A* path suggestion written in C
## Requirements
- C17
- Makefile or CMake
- SDL2 library
## Modes
- User operated 
- Basic X and Y based state AI
- Forward Pass Neural Network (9 input,3 hidden -> 64 each, 4 output)
- A* suggestion while user operated (paints the tiles yellow to represent the shortest path)
## Notes
- the weights and biases .csv files contain the weights and biases for the neural network. Anyone can easily after training switch their own values instead of the ones provided
- the given code only represents the written code
- the neural network code does not contain the back-propagation because it does not
- Neural network was trained on the choices.csv that represent every choice to change direction when playing
## How to run
- git clone the repository
- write your own CMake or Makefile that will include all the linking to the SDL2 directory on your computer, or just clone an SDL2 template any copy and paste the .c and .h files inside the scripts directory
## Issues
- bad memory managment for the A*, leading to crashes when hitting the wall with the A* mode turned on
- sticky A* for drawing
- really stupid Snake behaviour when in Neural Network mode (tricky to get right even after 15 different hyperparamets tests)
## Possible additions
- Performance improvments (Multithreding and/or CUDA)
- Genetic algorithm
- Q Learining
## Images
![image](https://github.com/user-attachments/assets/d529bb49-0f57-4e8b-857a-501e559c24d1)
![image](https://github.com/user-attachments/assets/8aa7ce61-2b34-425b-8062-5524f7d9db2d)
![image](https://github.com/user-attachments/assets/770b802d-ad81-4434-bae0-3c3059cd1ac3)



