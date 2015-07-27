.PHONY: all clean
APP:=pttc-emu
TEST:=read_test
MAIN_DIR:=.
MAIN_SRC:=master.c
TEST_SRC:=reader.c
CFLAGS:=-I$(MAIN_DIR)/include
LIBS:=-lpthread

all: $(APP) $(TEST)

$(APP): $(MAIN_SRC:.c=.o)
	gcc -o $@ $^ $(LIBS)

$(TEST): $(TEST_SRC:.c=.o)
	gcc -o $@ $^

$(TEST_SRC:.c=.o): %.o: $(MAIN_DIR)/src/%.c
	gcc -o $@ -c $< $(CFLAGS)

$(MAIN_SRC:.c=.o): %.o: $(MAIN_DIR)/src/%.c
	gcc -o $@ -c $< $(CFLAGS)

clean:
	rm -fr *.o $(APP) $(TEST)
