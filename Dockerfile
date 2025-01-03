# Этап сборки
FROM ubuntu:22.04 AS builder

# Установка необходимых пакетов
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Установка Conan
RUN pip3 install conan==1.60.1

# Настройка Conan
RUN conan profile new default --detect && \
    conan profile update settings.compiler.libcxx=libstdc++11 default

# Копирование файлов проекта
WORKDIR /app
COPY . .

# Сборка проекта с помощью Conan
RUN mkdir build && cd build && \
    conan install .. --build=missing && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --config Release

# Финальный образ
FROM ubuntu:22.04

# Установка runtime зависимостей
RUN apt-get update && apt-get install -y \
    libssl3 \
    libpq5 \
    && rm -rf /var/lib/apt/lists/*

# Копирование собранного приложения
COPY --from=builder /app/build/bin/webDataBase /usr/local/bin/
COPY --from=builder /app/config.json /etc/webDataBase/config.json

# Определение рабочей директории
WORKDIR /usr/local/bin

# Открываем порт
EXPOSE 8080

# Запуск приложения
CMD ["webDataBase"]
