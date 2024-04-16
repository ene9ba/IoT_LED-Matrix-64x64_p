"""
Custom pioasm compiler script for platformio.
(c) 2022 by P.Z.

"""
from os.path import join
import glob
import sys

Import("env")

platform = env.PioPlatform()
PROJ_SRC = env["PROJECT_SRC_DIR"]
PIO_FILES = glob.glob(join(PROJ_SRC, '*.pio'), recursive=True)
print("piofiles: \n")
print(PIO_FILES)

if PIO_FILES:
    print("==============================================")
    print('PIO ASSEMBLY COMPILER')
    try:
        #PIOASM_DIR = platform.get_package_dir("tool-pioasm-rp2040-earlephilhower")
        PIOASM_DIR = "scripts"
    except:
        print("tool pioasm not found!")
        print("please install it in scripts folder")
        sys.exit()

    #PIOASM_EXE = join(PIOASM_DIR, "pioasm")
    PIOASM_EXE = "tools/pioasm"    
    print("pio files found:")
    for filename in PIO_FILES:
        env.Execute(PIOASM_EXE + f' -o c-sdk {filename} {filename}.h')
    print("==============================================")