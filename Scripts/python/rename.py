import os


for dirpath, _, filenames in os.walk(os.getcwd()):
    for filename in filenames:
        ext = os.path.splitext(filename)
        path_no_ext = os.path.join(dirpath, ext[0])
        if ext[1] == '.vs':
            os.rename(path_no_ext + '.vs', path_no_ext + '.vert')
        elif ext[1] == '.fs':
            os.rename(path_no_ext + '.fs', path_no_ext + '.frag')
        elif ext[1] == '.gs':
            os.rename(path_no_ext + '.gs', path_no_ext + '.geom')
