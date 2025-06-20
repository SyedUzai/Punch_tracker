set(srcs 
	"i2c_eeprom_main.c"
	"LSM6DS3.c"
	"freertos_tasks.c"
	"udp_sender.c"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
