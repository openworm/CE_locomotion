dir_name = None
def rename_file(file_name):
    if dir_name is None: return file_name
    return dir_name + '/' + file_name