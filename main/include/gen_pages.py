import os

pages = os.listdir("./html_pages")

f = open("html_pages.h", "w")
f.write("// This header auto generated do not edit directly.\n")
f.write("#ifndef __HTML_PAGES__\n#define __HTML_PAGES__\n\n")

for page in pages:
    var_file = open(f"./html_pages/{page}", "r")
    var_name = page.replace('.', '_')
    var_content = var_file.read().replace("\n","").replace("\t", "").replace('"', '\\"')
    f.write(f'const char *{var_name} = "{var_content}";\n\n')
    var_file.close()

f.write("#endif // __HTML_PAGES__")
f.close()
