.PHONY: build main main2 map vector clean

BUILD_DIR := build
DATA ?= dummy1.json

build:
	@cmake -S . -B $(BUILD_DIR) >/dev/null
	@cmake --build $(BUILD_DIR) -- -s

main:
	@cmake -S . -B $(BUILD_DIR) >/dev/null
	@cmake --build $(BUILD_DIR) --target main_map -- -s
	@DATA_FILE=$(DATA) ./$(BUILD_DIR)/main_map

main2:
	@cmake -S . -B $(BUILD_DIR) >/dev/null
	@cmake --build $(BUILD_DIR) --target main_vector -- -s
	@DATA_FILE=$(DATA) ./$(BUILD_DIR)/main_vector

map: main

vector: main2

clean:
	@cmake --build $(BUILD_DIR) --target clean
