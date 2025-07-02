cmake_minimum_required(VERSION 3.29)
project(Space_Invaders)

set(CMAKE_CXX_STANDARD 14)


include_directories(include)

add_executable(Space_Invaders
    include/Bullet.h
    include/Enemy.h
    include/GameObject.h
    include/Player.h
    src/bullet.cpp
    src/enemy.cpp
    src/game.cpp
    src/main.cpp
        src/GameObject.cpp)
