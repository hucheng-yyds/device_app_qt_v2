HEADERS += \
    $$PWD/icmodule.h \
    $$PWD/libHTRC.h \
    $$PWD/spicard.h \
    $$PWD/spicard/Crc16.h \
    $$PWD/spicard/cse7761.h \
    $$PWD/spicard/debugApp.h \
    $$PWD/spicard/GeneralModule.h \
    $$PWD/spicard/gpio_defs.h \
    $$PWD/spicard/iso14443.h \
    $$PWD/spicard/main.h \
    $$PWD/spicard/mcu.h \
    $$PWD/spicard/nb.h \
    $$PWD/spicard/sky1311_card.h \
    $$PWD/spicard/sky1311Instan.h \
    $$PWD/spicard/sky1311t.h \
    $$PWD/spicard/sky1311t_reg.h \
    $$PWD/spicard/sky1311tDriver.h \
    $$PWD/spicard/spicard.h \
    $$PWD/spicard/typedefs.h \
    $$PWD/spicard/types.h \
    $$PWD/spicard/user_cse7761.h \
    $$PWD/spicard/user_cse7761_driver.h \
    $$PWD/spicard/userdef.h

SOURCES += \
    $$PWD/icmodule.cpp \
    $$PWD/spicard/Crc16.c \
    $$PWD/spicard/iso14443a.c \
    $$PWD/spicard/iso14443b.c \
    $$PWD/spicard/mcu.c \
    $$PWD/spicard/mifare1.c \
    $$PWD/spicard/sky1311_card.c \
    $$PWD/spicard/sky1311Instan.c \
    $$PWD/spicard/sky1311t.c \
    $$PWD/spicard/sky1311tDriver.c \
    $$PWD/spicard/spicard.c
