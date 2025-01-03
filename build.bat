@echo off
REM Создаем директорию для сборки
if not exist build mkdir build
cd build

REM Устанавливаем зависимости через Conan
conan install .. --build=missing

REM Конфигурируем и собираем проект
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
