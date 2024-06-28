nes_ss = ss.source_set()
nes_ss.add(files('boot.c'))
nes_ss.add(when: 'CONFIG_NES_MCU', if_true: files('nes_mcu.c'))
nes_ss.add(when: 'CONFIG_NES_BOARD', if_true: files('nes.c'))

# 必须指定架构，和config的一样
hw_arch += {'nes': nes_ss}
