gen = [
  decodetree.process('insn.decode', extra_args: [ '--decode', 'decode_insn',
                                                  '--insnwidth', '16' ])
]

nes_ss = ss.source_set()
nes_system_ss = ss.source_set()

nes_ss.add(gen)
nes_ss.add(files(
  'translate.c',
  'helper.c',
  'cpu.c',
  'gdbstub.c',
  'disas.c'))

nes_system_ss.add(files('machine.c'))

# 必须指定架构，和config的一样
target_arch += {'nes': nes_ss}
target_softmmu_arch += {'nes': nes_system_ss}
