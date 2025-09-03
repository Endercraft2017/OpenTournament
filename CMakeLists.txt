cmake_minimum_required(VERSION 3.16)
project(OpenTournament VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable automatic MOC, RCC and UIC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# For MSVC, make sure /Zc:__cplusplus is passed
if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Database.cpp
    src/Player.cpp
    src/Match.cpp
    src/Tournament.cpp
)

set(HEADERS
    src/MainWindow.h
    src/Database.h
    src/Player.h
    src/Match.h
    src/Tournament.h
)

add_executable(OpenTournament ${SOURCES} ${HEADERS})

target_link_libraries(OpenTournament PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

install(TARGETS OpenTournament DESTINATION bin)
install(FILES tournament.db DESTINATION share/OpenTournament)