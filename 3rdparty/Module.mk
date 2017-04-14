
libudis86_TEMP_OUTPUT = $(addprefix $(libudis86_DIR),udis86/libudis86/.libs/libudis86.a)

define libudis86_BINARY_RULES
$$(libudis86_CONFIG_DIR):
	mkdir -p $$(libudis86_CONFIG_DIR)

$$(libudis86_TEMP_OUTPUT):
	cd $$(libudis86_DIR)udis86 && ./autogen.sh && ./configure --enable-static
	$$(MAKE) -C $$(libudis86_DIR)/udis86 clean
	$$(MAKE) -C $$(libudis86_DIR)/udis86 CFLAGS=-fPIC
	cp $$(libudis86_TEMP_OUTPUT) $$(libudis86_BINARY)

$$(libudis86_BINARY): $$(libudis86_TEMP_OUTPUT) $$(libudis86_CONFIG_DIR)
 
libudis86_TEMP_CLEAN:
	cd $(libudis86_DIR)udis86
	$$(MAKE) -C $$(libudis86_DIR)udis86 clean

libudis86_CLEAN: libudis86_TEMP_CLEAN

#.PHONY: libudis86
#libudis86: $$(libudis86_COPY)

endef # libudis86_BINARY_RULES

$(eval $(call CREATE_MODULE,libudis86,ARC))
