cmake_minimum_required(VERSION 3.5)
project(webdatabase_backend CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Находим необходимые пакеты
find_package(Drogon REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

# Добавляем исходные файлы
file(GLOB_RECURSE SOURCES "src/*.cc" "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp")
# # Добавляем новые исходные файлы
# set(SOURCES
#     ${SOURCES}
#     ${PROJECT_SOURCE_DIR}/src/controllers/SchemaController.cc
#     ${PROJECT_SOURCE_DIR}/src/models/Schema.cc
# )

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключаем библиотеки
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    Drogon::Drogon
    OpenSSL::SSL 
    OpenSSL::Crypto
    jsoncpp
)

# Включаем директории с заголовочными файлами
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Копируем конфигурационные файлы
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
