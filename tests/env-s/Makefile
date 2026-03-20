XLEN ?= 64
src_dir := ../riscv-tests/isa

include $(src_dir)/rv64ui/Makefrag
# include $(src_dir)/rv64um/Makefrag
rv64ui_s_tests = $(addprefix rv64ui-s-, $(rv64ui_sc_tests))
# rv64um_s_tests = $(addprefix rv64um-s-, $(rv64um_sc_tests))

default: all

RISCV_PREFIX ?= riscv$(XLEN)-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)gcc
RISCV_GCC_OPTS ?= -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles
RISCV_OBJDUMP ?= $(RISCV_PREFIX)objdump --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.text.init --section=.data
RISCV_OBJCOPY ?= $(RISCV_PREFIX)objcopy 

vpath %.S $(src_dir)

%.dump: %
	$(RISCV_OBJDUMP) $< > $@

%.hex: %
	$(RISCV_OBJCOPY) $< -O binary $@

define compile_template

$$($(1)_s_tests): $(1)-s-%: $(1)/%.S
	$$(RISCV_GCC) $(2) $$(RISCV_GCC_OPTS) -I. -I$(src_dir)/macros/scalar -T./link.ld $$< -o $$@
$(1)_tests += $$($(1)_s_tests)

$(1)_tests_dump = $$(addsuffix .dump, $$($(1)_tests))

$(1): $$($(1)_tests_dump)

.PHONY: $(1)

tests += $$($(1)_tests)

endef

$(eval $(call compile_template,rv64ui,-march=rv64g -mabi=lp64d))
# $(eval $(call compile_template,rv64um,-march=rv64g -mabi=lp64d))

tests_dump = $(addsuffix .dump, $(tests))
tests_hex = $(addsuffix .hex, $(tests))

junk += $(tests) $(tests_dump) $(tests_hex)

all: $(tests_dump) $(tests_hex)

clean:
	rm -rf $(junk)
