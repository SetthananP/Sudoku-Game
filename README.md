# 🧩 Sudoku Game

## 📝 Project Description
Developed a **Sudoku game** using **C**, utilizing a **server-client model** with **threaded programming** to enable **real-time interactions** between players.

![Build Status](https://img.shields.io/badge/build-Complete-brightgreen)

## Table of Contents
- [Features](#features)
- [Screenshots](#screenshots)
- [Installation](#installation)

## Features
- 🌐 **Server-client architecture**: The game is built on a server-client model, ensuring smooth communication.
- 🧵 **Threaded programming**: Each player interaction is handled in separate threads, improving performance and responsiveness.

## Screenshots
**1. Example Game**<br><dd>In the terminal on the right, we have the server side of the system. This server is responsible for checking the columns and values to see if they are correct. It waits for input from the client side, which is on the left. The client sends rows of numbers, for example, 1 1 2.<br>
<dd>The server receives these rows and verifies whether the numbers are correct according to the game's rules. If the input is correct, the server will continue to process the next set of data. This process repeats until all the data has been checked.

<img src="https://github.com/SetthananP/Sudoku-Game/blob/main/playgame.png?raw=true" alt="Example Game" width="900"/>


## Installation

To run this project locally:

1. Clone the repository:
   ```bash
   git clone https://github.com/SetthananP/Sudoku-Game.git

2. Navigate into the project directory:
   ```bash
   cd Sudoku-Game

3. Compile the server and client:
   ```bash
   gcc serverProject.c -o serverProject
   gcc clientProject.c -o clientProject

4. Run the server:
   ```bash
   ./serverProject

5. Open another terminal and run the client:
   ```bash
   ./clientProject
   
