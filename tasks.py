from invoke import task
import os
import shutil

@task
def build(c, verbose=False, configure=False):
    if configure:
        c.run("cmake -S . -B build")
    build_cmd = "cmake --build build --target all"
    if verbose:
        build_cmd += " -v"
    c.run(build_cmd)

@task
def clean(c):
    build_cmd = "cmake --build build --target clean"
    c.run(build_cmd)
    shutil.rmtree('build', ignore_errors=True)
    shutil.rmtree('CMakeFiles', ignore_errors=True)
