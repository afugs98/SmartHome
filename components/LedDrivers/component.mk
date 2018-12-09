# Component makefile for WS2812FX

INC_DIRS += $(LedDrivers_ROOT).

# args for passing into compile rule generation
LEDDRIVERS_INC_DIR =  $(LedDrivers_ROOT).
LEDDRIVERS_SRC_DIR =  $(LedDrivers_ROOT).

$(eval $(call component_compile_rules, LedDrivers))