cmake_minimum_required(VERSION 3.21)
project(RestaurantORM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(RestaurantORM
    src/main.cpp
    src/CompareDemo.cpp
    src/Demos.cpp
    src/SQLiteDiningTableRepository.cpp
    src/OrderStore.cpp
    src/TestData.cpp
)

target_include_directories(RestaurantORM
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RestaurantORM
    PRIVATE
    sqlite_orm::sqlite_orm
    unofficial::sqlite3::sqlite3
)