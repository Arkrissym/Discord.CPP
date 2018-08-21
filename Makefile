VERSION = 0.2.1
LIB_NAME = discord_cpp
LIB = lib$(LIB_NAME).so

SRC_DIR := Discord.C++
OBJ_DIR := build
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o, $(SRC_FILES))
HEADER_FILES := $(wildcard $(SRC_DIR)/*.h)

CXXFLAGS := -std=c++14 -fPIC -Wall -Wextra -O2

all: $(OBJ_FILES)
	g++ -shared -o $(LIB).$(VERSION) $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "CC $<"
	@g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

install:
	mkdir /usr/local/include/$(LIB_NAME)
	cp -f -t /usr/local/include/$(LIB_NAME) $(HEADER_FILES)
	cp -f  $(LIB).$(VERSION) /usr/local/lib/
	ldconfig

uninstall:
	rm -rf /usr/local/include/$(LIB_NAME)
	rm -f /usr/local/lib/$(LIB).*
	ldconfig

test:
	@echo "CC $@"
	@g++ -o $@ -g -O2 -std=c++14 -Wall test_bot/main.cpp -l$(LIB_NAME) -lboost_system -lcrypto -lssl -lcpprest

.PHONY: all

clean:
	@rm -f $(OBJ_FILES) $(LIB).* test
	@rmdir $(OBJ_DIR)
