#!/usr/bin/env python3
import os

os.chdir("/home/akbay/Documents/Tarim_sunum/IOT/station/main/include")
pages = os.listdir("./html_pages")

fheader = open("html_pages.h", "w")
fheader.write("// This header auto generated do not edit directly.\n")
fheader.write("#ifndef __HTML_PAGES__\n#define __HTML_PAGES__\n\n")

fsrc = open("../html_pages.c", "w")
fsrc.write("// This source file auto generated do not edit directly.\n")
fsrc.write("#include \"html_pages.h\"\n\n")

for page in pages:
    var_file = open(f"./html_pages/{page}", "r")
    var_name = page.replace('.', '_')
    var_content = var_file.read().replace("\n","").replace("\t", "").replace('"', '\\"')
    fsrc.write(f'const char *{var_name} = "\\r\\n{var_content}\\r\\n\\r\\n";\n')
    fheader.write(f"extern const char *{var_name};\n")
    var_file.close()

fheader.write("#endif // __HTML_PAGES__")
fheader.close()
fsrc.close()
