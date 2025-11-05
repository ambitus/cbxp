# Contributing to CBXP

Thank you for taking the time to contribute to CBXP!
The following are a set of guidelines to help you contribute.

**Table Of Contents**

* [Before Getting Started](#before-getting-started)

* [Ways to Contribute](#ways-to-contribute)

  * [Coding](#coding)

  * [pre-commit Hooks](#pre-commit-hooks)

  * [Adding New Functionality](#adding-new-functionality)

  * [Branch Naming Conventions](#branch-naming-conventions)

  * [Fixing Bugs](#fixing-bugs)

  * [Testing](#testing)

    * [Python Interface Testing](#python-interface-testing)

    * [Shell Interface Testing](#shell-interface-testing)

* [Style Guidelines](#style-guidelines)

* [Static Code Analysis](#static-code-analysis)

* [Contribution checklist](#contribution-checklist)

* [Found a bug?](#found-a-bug)

## Before Getting Started

> :warning: _All code contributed must be made under an Apache 2 license._
>
> :warning: _All contributions must be accompanied by a [Developer Certification of Origin (DCO) signoff](https://github.com/openmainframeproject/tsc/blob/master/process/contribution_guidelines.md#developer-certificate-of-origin)._

## Ways to Contribute

There are many ways to contribute to the project. You can write code, work on the documentation, provide tests, report bugs or provide suggestions for improvement.

### Coding

If you want to write code, a good way to get started is by looking at the issues section of this repository. Look for the **Good First Issue** tag. Good First Issues are great as a first contribution.

### pre-commit Hooks
To ensure `clang-format` and `cppcheck` are always run against your code on **every commit**, set up the **pre-commit hooks**.

* Install [`pre-commit`](https://pre-commit.com/).
* Setup **pre-commit Hooks**:

  ```shell
  pre-commit install
  ```

### Adding New Functionality

If you want to continube new functionality, open a GitHub pull request against the `dev` branch with your changes. In the PR, make sure to clearly document the new functionality including why it is valuable.

### Branch Naming Conventions

Code branches should use the following naming conventions:

* `wip/name` *(Work in progress branch that likely won't be finished soon)*
* `feat/name` *(Branch where new functionality or enhancements are being developed)*
* `bug/name` *(Branch where one or more bugs are being fixed)*
* `junk/name` *(Throwaway branch created for experimentation)*

### Fixing Bugs

If you fix a bug, open a GitHub pull request against the `dev` branch with the fix. In the PR, make sure to clearly describe the problem and the solution.

### Testing

CBXP is tested using automated functional tests. Test cases for new functionality and bug fixes should be added to [`tests/test.py`](tests/test.py) and [`tests/test.sh`](tests/test.sh).

#### Python Interface Testing
1. Add new test cases to [`tests/test.py`](tests/test.py).
2. Build and install the CBXP Python package. Both the **wheel** and **sdist** distributions for all [Python versions supported by CBXP](README.md#minimum-zos--language-versions) should be tested.

   ```shell
   # Build
   python3 -m build
   # Install sdist
   python3 -m pip install dist/<cbxp sdist>.tar.gz
   # Install wheel
   python3 -m pip install dist/<cbxp wheel>.whl
   ```

3. Run the test suite.

   ```shell
   python3 ./tests/test.py
   ```

#### Shell Interface Testing
1. Add new tests cases to [`tests/test.sh`](tests/test.sh).
2. Run the test suite using `cmake` and `gmake`.

   ```shell
   cmake .
   gmake test
   ```

## Style Guidelines

:bulb: _`clang-format` can be setup to run automatically using the [pre-commit Hooks](#pre-commit-hooks)._

The use of the `clang-format` code formatter is required.

The following code style conventions should be followed:
* Varible names should use snake case *(i.e., `my_variable`)*.
* Pointer variables should start with `p_` *(i.e., `p_my_pointer`)*.
* Class variables should end with an `_` to help differentiate between class variables and local function variables *(i.e., `my_class_variable_`)*.
* Class names should use pascal case *(i.e., `MyClass`)*.
* Function names should use camel case *(i.e., `myFunction()`)*.
* When calling a class function within the same class that function is a member of, the following syntax should be used to make it clear that a function within the same class is being called.

  ```cpp
  MyClass::myFunction();
  ```

* Structs should use the following naming convention.

  ```cpp
  typedef struct {
    int member_1;
    char member_2[5];
  } my_struct_t;
  ```

## Static Code Analysis

:bulb: _`cppcheck` can be setup to run automatically using the [pre-commit Hooks](#pre-commit-hooks)._

`cppcheck` will be run against all code contributions to ensure that contributions don't inadvertently introduce any vulnerabilities or other significant issues. All contributions must have no `cppcheck` complaints. False positives and minor complaints may be [suppressed](http://cppcheck.net/manual.html#inline-suppressions) when it makes sense to do so, but this should only be done sparingly. All `cppcheck` comlpaints should be evaluated and corrected when it is possible and makes sense to do so. You can run `cppcheck` by running `gmake check`.

## Contribution checklist

When contributing to CBXP, make sure to complete all applicable tasks in the following contribution checklist:

* Make any necessary updates to `pyproject.toml`.
* Make any necessary updates to `README.md`.
* Ensure that you have **pre-commit Hooks** setup to ensure that `clang-format` and `cppcheck` are run against the code for every commit you make.
* Run `cppcheck` static code analysis by running `gmake check`.
* Check for style violations using `clang-format` by running `gmake lint`.
* Format the CBXP code using `clang-format` by running `gmake format`.

## Found a bug?

If you find a bug in the code, please open the an issue.
In the issue, clearly state what is the bug, and  any other details that can be helpful.
