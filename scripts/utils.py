import os

def GetFiles(path, ext):
    files = []

    for root, directories, filenames in os.walk(path):
        for filename in filenames:
            if filename.endswith(ext):
                files.append(os.path.join(root, filename))

    return files

def IsUpToDate(output_file, input_file):
    return os.path.exists(output_file) and (os.path.getmtime(output_file) > os.path.getmtime(input_file))