# Snake++

A snake game using C++ for the TI-84 Plus CE

## Game modes
### Classic
Collect food for points, making sure to not hit walls or yourself as you grow

![Snake++ Classic Gameplay](game-animated.png)

### Wrap
Collect food for points, traversing from one side to another, and making sure to not hit yourself as you grow

![Snake++ Wrap mode Gameplay](game-wrap-animated.png)

### Winged
Catch diagonally moving food and try not to bump into yourself in the hunt

![Snake++ Wrap mode Gameplay](game-winged-animated.png)

> Other gamemodes will come in later releases

## Story

> Coming in a later release

## Store
![Store](Store-UI-1.png)

Allows the player to buy and select snake skins using collected golden apples (from playing Snake++)

## Options
### UI
Allows for a fully-customizable themeing control with saving using an `AppVar`
![](Menu-UI-Themes.png)

### Stats

Shows stats from playing, like the best score in each mode, and how much food you have eaten

![Snake++ Classic Gameplay](game-stats.png)

---
Using the [TI-84 Plus CE Calculator Toolchain](https://github.com/CE-Programming/toolchain)

## Building

Clone the repository into a local folder

### Manual

Follow the building instruction on the [CE C/C++ Toolchain](https://ce-programming.github.io/toolchain/static/getting-started.html) documnetation

### IDE

- Install [premake5](https://premake.github.io/)
- Make sure your IDE has build commands and a project can be created by premake

```bash
premake5 [ide name] 
```