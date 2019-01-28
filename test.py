f = open("files.d", "r")

lines = f.readlines()

for i in range(len(lines)):
    lines[i] = lines[i].replace(" \\\n", "")

print(lines)

f.close()