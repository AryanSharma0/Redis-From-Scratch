CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -pthread -MMD -MP -O2 -Iinclude

SRC_DIR := src
BUILD_DIR := build

COMMON_SRCS := $(filter-out $(SRC_DIR)/main.cpp $(SRC_DIR)/RedisClient.cpp,$(wildcard $(SRC_DIR)/*.cpp))

SERVER_SRCS := $(SRC_DIR)/main.cpp $(COMMON_SRCS)
SERVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRCS))

CLIENT_SRCS := $(SRC_DIR)/RedisClient.cpp
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRCS))

SERVER_TARGET := my_redis_server
CLIENT_TARGET := redis_cli

SERVER_DEPS := $(SERVER_OBJS:.o=.d)
CLIENT_DEPS := $(CLIENT_OBJS:.o=.d)


.PHONY: all clean rebuild run-server run-client

all: $(SERVER_TARGET) $(CLIENT_TARGET)


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $@


$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $@


-include $(SERVER_DEPS)
-include $(CLIENT_DEPS)


clean:
	rm -rf $(BUILD_DIR) $(SERVER_TARGET) $(CLIENT_TARGET)


rebuild: clean all


run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)


run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)