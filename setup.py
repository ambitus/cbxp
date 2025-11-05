"""Build Python extension."""

import os
from glob import glob

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


def main():
    """Python extension build entrypoint."""
    os.environ["CC"] = "ibm-clang64"
    os.environ["CFLAGS"] = "-std=c11"
    os.environ["CXX"] = "ibm-clang++64"
    os.environ["CXXFLAGS"] = "-std=c++17"
    setup_args = {
        "ext_modules": [
            Extension(
                "cbxp._C",
                sources=(
                    glob("cbxp/**/*.cpp")
                    + [file for file in glob("cbxp/*.cpp") if file != "cbxp/main.cpp"]
                    + ["cbxp/python/_cbxp.c"]
                ),
                include_dirs=(
                    glob("cbxp/**/")
                    + [
                        "cbxp",
                        "externals",
                        "/usr/include/zos"
                    ]
                ),
                extra_link_args=[
                    "-m64",
                    "-Wl,-b,edit=no",
                ],
                extra_compile_args=[
                    "-fzos-le-char-mode=ascii",
                    "-Wno-trigraphs"
                ]
            ),
        ],
        "cmdclass": {"build_ext": build_ext},
    }
    setup(**setup_args)


if __name__ == "__main__":
    main()
