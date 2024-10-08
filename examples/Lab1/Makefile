CС=gcc

SOURCES=task_1.c task_2.c
TEST_TEMPLATES_DIR=../../test_templates
EXECUTABLES=$(SOURCES:.c=.a)
BASH=bash
SUCCESS_CMD=$(BASH) $(TEST_TEMPLATES_DIR)/success_cmd
SUCCESS_OFILE=$(BASH) $(TEST_TEMPLATES_DIR)/success_ofile
FAILED_CMD=$(BASH) $(TEST_TEMPLATES_DIR)/failed_cmd
FAILED_OFILE=$(BASH) $(TEST_TEMPLATES_DIR)/failed_ofile


.PHONY: all test clean

all: $(EXECUTABLES)

%.a : %.c
	$(CC) $< -o $@

clean:
	rm -r -f *.a

test_command_args_1:
	@$(FAILED_CMD) "task_1.a" -1 "Incorrect args count"
	@echo "TEST 1 PASSED"

test_commad_success_1:
	@$(SUCCESS_CMD) "task_1.a 12" "12"
	@echo "TEST 2 PASSED"

test_commad_success_ofile_1:
	@$(SUCCESS_OFILE) "task_1.a 12" "output.txt"
	@echo "TEST 3 PASSED"


test: test_command_args_1 test_commad_success_1 test_commad_success_ofile_1
	


