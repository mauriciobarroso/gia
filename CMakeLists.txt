idf_component_register(SRCS "gia.c" "sensirion_gas_index_algorithm/sensirion_gas_index_algorithm.c"
                    INCLUDE_DIRS "include" "sensirion_gas_index_algorithm"
                    REQUIRES sht4x sgp41)
