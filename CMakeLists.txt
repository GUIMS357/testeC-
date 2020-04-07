cmake_minimum_required (VERSION 1.13)

# passa a permitir politicas de versões que sejam 2.4 ou inferior.
cmake_policy(VERSION 2.4[...3.17])


# nome e versão do projeto
project(FiltroDeSobel VERSION 1.0)

# add the executable

add_executable(FiltroDeSobel Main.cpp)

#eigen

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

target_link_libraries (FiltroDeSobel Eigen3::Eigen)

#png

find_package(PNG REQUIRED)

target_link_libraries(FiltroDeSobel PNG::PNG)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)





