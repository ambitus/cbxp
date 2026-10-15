"""Build Python extension."""

import os
from glob import glob
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


def _escape_for_macro(text: str) -> str:
    """Collapse whitespace and escape characters unsafe in a C string literal."""
    return (
        text.replace("\\", "\\\\")
        .replace('"', '\\"')
        .replace("\n", " ")
        .replace("\r", "")
    )


def generate_headers():
    """Generate .hpp headers from .json files, mirroring the CMake process."""
    # Schema header
    schema_json = _escape_for_macro(
        Path("cbxp/schemas/control_block_v1.json").read_text(),
    )
    Path("cbxp/schemas/control_block_v1.hpp").write_text(
        "#ifndef CBXP_SCHEMA_CONTROL_BLOCK_V1_JSON_HPP\n"
        "#define CBXP_SCHEMA_CONTROL_BLOCK_V1_JSON_HPP\n"
        "\n"
        f'#define CBXP_SCHEMA_JSON "{schema_json}"\n'
        "\n"
        "#endif // CBXP_SCHEMA_CONTROL_BLOCK_V1_JSON_HPP\n",
    )

    # Mapping headers
    for json_path in Path("cbxp/mappings").glob("*.json"):
        name = json_path.stem.upper()
        guard = f"{name}_JSON_HPP"
        macro = f"{name}_JSON"
        mapping_json = _escape_for_macro(json_path.read_text())
        json_path.with_suffix(".hpp").write_text(
            f"#ifndef {guard}\n"
            f"#define {guard}\n"
            "\n"
            f'#define {macro} "{mapping_json}"\n'
            "\n"
            f"#endif // {guard}\n",
        )


def main():
    """Python extension build entrypoint."""
    generate_headers()
    os.environ["CC"] = "ibm-clang64"
    os.environ["CFLAGS"] = "-std=c11"
    os.environ["CXX"] = "ibm-clang++64"
    os.environ["CXXFLAGS"] = "-std=c++17"
    setup_args = {
        "ext_modules": [
            Extension(
                "cbxp._C",
                sources=(
                    [
                        file
                        for file in glob("cbxp/**/*.cpp")
                        if file not in glob("cbxp/cli/*.cpp")
                    ]
                    + glob("cbxp/*.cpp")
                    + glob("externals/json-schema-validator/*.cpp")
                    + ["cbxp/python/_cbxp.c"]
                ),
                include_dirs=(
                    glob("cbxp/**/")
                    + glob("externals/*/")
                    + ["cbxp", "/usr/include/zos"]
                ),
                extra_link_args=[
                    "-m64",
                    "-Wl,-b,edit=no",
                ],
                extra_compile_args=(
                    [
                        "-O2",
                        "-fzos-le-char-mode=ascii",
                        "-fstack-protector-strong",
                        "-Wno-trigraphs",
                    ]
                    + [f"-include{h}" for h in glob("cbxp/schemas/*.hpp")]
                    + [f"-include{h}" for h in glob("cbxp/mappings/*.hpp")]
                ),
            ),
        ],
        "cmdclass": {"build_ext": build_ext},
    }
    setup(**setup_args)


if __name__ == "__main__":
    main()
