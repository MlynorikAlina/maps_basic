#!/bin/bash
echo "Hi"

# Проверяем, что переданы оба обязательных аргумента
if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Использование: $0 <путь_к_osm.pbf> <папка_для_вывода>"
    echo "Пример: $0 /path/to/file.osm.pbf /path/to/extracted_tiles"
    exit 1
fi

# Назначаем аргументы в понятные переменные
PBF="$1"
OUTPUT_DIR="$2"

# Проверяем физическое наличие файла базы данных
if [ ! -f "$PBF" ]; then
    echo "Ошибка: Файл базы данных не найден по пути: $PBF"
    exit 1
fi

echo "Начало распаковки из: $PBF"
echo "Целевая папка: $OUTPUT_DIR"

tilemaker --process ./process-openmaptiles.lua  --config ./config-openmaptiles.json  --input $PBF --output $OUTPUT_DIR --store tmp
