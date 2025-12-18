PACKAGES=libc libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata bios
PACKAGE_DIRS=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libc /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libcompiler_rt /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libbase /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libfatfs /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitespi /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitedram /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libliteeth /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitesdcard /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitesata /home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/bios
LIBS=libc libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata
TRIPLE=riscv64-unknown-elf
CPU=picorv32
CPUFAMILY=riscv
CPUFLAGS=-mno-save-restore -march=rv32i2p0_m     -mabi=ilp32 -D__picorv32__ 
CPUENDIANNESS=little
CLANG=0
CPU_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/cores/cpu/picorv32
SOC_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc
PICOLIBC_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/pythondata-software-picolibc/pythondata_software_picolibc/data
PICOLIBC_FORMAT=integer
COMPILER_RT_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/pythondata-software-compiler_rt/pythondata_software_compiler_rt/data
export BUILDINC_DIRECTORY
BUILDINC_DIRECTORY=/home/mateus/Documentos/embarca/colorlight-i9-examples/up/build/colorlight_i5/software/include
LIBC_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libc
LIBCOMPILER_RT_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libcompiler_rt
LIBBASE_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libbase
LIBFATFS_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libfatfs
LIBLITESPI_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitespi
LIBLITEDRAM_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitedram
LIBLITEETH_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/libliteeth
LIBLITESDCARD_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitesdcard
LIBLITESATA_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/liblitesata
BIOS_DIRECTORY=/home/mateus/Documentos/embarca/oss/litex/litex/litex/soc/software/bios
LTO=0
BIOS_CONSOLE_FULL=1