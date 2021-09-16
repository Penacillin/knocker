from invoke import task

@task
def build(c, verbose=False, configure=False):
    if configure:
        c.run("cmake --configure  --build build")
    build_cmd = "cmake --build build --target all"
    if verbose:
        build_cmd += " -v"
    c.run(build_cmd)

@task
def clean(c):
    build_cmd = "cmake --build build --target clean"
