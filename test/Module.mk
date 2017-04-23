salsasm_test_HEADERS :=
salsasm_test_SOURCES := main.cpp print.cpp

AS := yasm

salsasm_test_HEADER_DIRS += 3rdparty/gtest-1.6.0 3rdparty/gtest-1.6.0/include 3rdparty/udis86
salsasm_test_LIBS += -lstdc++ -lpthread

salsasm_test_DEPENDS += libsalsasm libudis86

$(eval $(call CREATE_MODULE,salsasm_test,EXE))

define SALSASM_TEST_ASM

$(salsasm_test_CONFIG_DIR)/$(1)16.bin: $$(salsasm_test_DIR)/$(1).asm $$(salsasm_test_DIR)/test.inc $(MAKEFILE_LIST)
	$(AS) -w -DARCH=16 -o $$@ $$<
$(salsasm_test_CONFIG_DIR)/$(1)32.bin: $$(salsasm_test_DIR)/$(1).asm $$(salsasm_test_DIR)/test.inc $(MAKEFILE_LIST)
	$(AS) -w -DARCH=32 -o $$@ $$<
$(salsasm_test_CONFIG_DIR)/$(1)64.bin: $$(salsasm_test_DIR)/$(1).asm $$(salsasm_test_DIR)/test.inc $(MAKEFILE_LIST)
	$(AS) -w -DARCH=64 -o $$@ $$<

$(FINAL_OUT_DIR)/$(1)16.bin: $$(salsasm_test_CONFIG_DIR)/$(1)16.bin
	cp $$< $$@
$(FINAL_OUT_DIR)/$(1)32.bin: $$(salsasm_test_CONFIG_DIR)/$(1)32.bin
	cp $$< $$@
$(FINAL_OUT_DIR)/$(1)64.bin: $$(salsasm_test_CONFIG_DIR)/$(1)64.bin
	cp $$< $$@

salsasm_test_CLEAN_FILES += $$(salsasm_test_CONFIG_DIR)/$(1)16.bin $$(FINAL_OUT_DIR)/$(1)16.bin
salsasm_test_CLEAN_FILES += $$(salsasm_test_CONFIG_DIR)/$(1)32.bin $$(FINAL_OUT_DIR)/$(1)32.bin
salsasm_test_CLEAN_FILES += $$(salsasm_test_CONFIG_DIR)/$(1)64.bin $$(FINAL_OUT_DIR)/$(1)64.bin

salsasm_test_BIN_FILES += $(FINAL_OUT_DIR)/$(1)16.bin $(FINAL_OUT_DIR)/$(1)32.bin $(FINAL_OUT_DIR)/$(1)64.bin

endef # SALSASM_TEST_ASM

salsasm_test_BIN_CLEAN:
	rm -f $$(salsasm_test_CLEAN_FILES)

salsasm_test_CLEAN: salsasm_test_BIN_CLEAN

$(eval $(call SALSASM_TEST_ASM,test_one_byte))
$(eval $(call SALSASM_TEST_ASM,test_two_byte))
$(eval $(call SALSASM_TEST_ASM,test_three_byte))

salsasm_test_BIN_FILES += $(FINAL_OUT_DIR)/BIOS-bochs-latest.bin
salsasm_test_CLEAN_FILES += $(FINAL_OUT_DIR)/BIOS-bochs-latest
$(FINAL_OUT_DIR)/BIOS-bochs-latest.bin: $(salsasm_test_DIR)/BIOS-bochs-latest
	cp $< $@

$(salsasm_test_COPY): $(salsasm_test_BIN_FILES)

