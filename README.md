# MICRO-LIBS

This repository contains a collection of libraries targeted for microcontrollers.

Each library has the following folder structure:
- `src/`
  - `*.c`
- `inc/`
  - `*.h`
- `test/`
  - `test-*.c`
  - `Makefile`
- `README.md`
- `CHANGELOG.md`
- `*.mk`

\
The library source code can be found entirely inside the `src` folder and the
header files can be found inside the `inc` diectory.

A small description of the library can be found inside the `README.md` as well as
all the information needed to use it inside your project.

Inside the `CHANGELOG` a list of changes made to the library can be found, sorted by date.

The `*.mk` file is an utility Makefile that can be included to facilitate the compilation process.

Inside the `test` folder a collection of source files are used for [unit testing](https://it.wikipedia.org/wiki/Unit_testing)
with the [Unity Test Framework](http://www.throwtheswitch.org/unity). \
This folder is usually needed during the development of the library and otherwise
it can be ignored.

Anyway, all the tests can be executed using the provided Makefile inside the `test` folder,
by running the `make test_all` command.

> [!NOTE]
> The content of the previously mentioned files (with the exception of the makefiles)
> have to be entirely handled by the developer of the library, and it is not guaranteed
> to be always complete and/or correct.

## Documentation

Check out the [docs](docs) folder if you need to know more about the implementation
and development information of the provided libraries.

The rendered markdown documentation can be found online on the [wiki website](https://wiki.eagletrt.it/micro-libs/micro-libs/).

## Usage

If you need to use one or more libraries from this repo, include it as a submodule
to keep it updated to the latest version.

```shell
git submodule add git@github.com:eagletrt/micro-libs.git
```

Then you can add the sources and headers files of the library to your project manually
or include the given makefile for each library as follow:

```make
include path/to/library/makefile

build:
    gcc $(LIBRARY_C_INCLUDE_DIRS_PREFIX) $(LIBRARY_C_SOURCES) main.c -o main
```

> [!NOTE]
> Avoid to compile and link all the files contained in this repo to your project.
> Use only the files contained in the `src` and `inc` folders of the libraries that you need.

## Developement

If you want to add a new library to this repo run the script `gen_library_template.sh`
located in the [scripts](scripts) folder providing the name of the library.

The library name is validated by the script and have to match the following restrictions:
- Be at least two characters long
- Contains only lower case letters, numbers or hypens('-')
- Start with a lower case letter
- End with a lower case letter or a number

> [!TIP]
> Avoid fancy or *hard-to-read* names, even though they are technically valid
> they will not make life easier for others.

Remember to **update**:
- The `README.md` if info about the usage of the library have changed
- The `CHANGELOG.md` every time a change is made to the library functionality
- The documentation if a change is made or ulterior info needs to be added.

Every time a new feature is added or an old one is removed in a file, **update**
the corresponding test file in the `test` folder. \
If you make any changes to the library, even small ones, **run** `make test_all` to ensure
that everything works correctly, if at least one test fails or the program crash,
**solve** the problem and re-run the test until each one is passed.

### Change Log

The change log can be useful when the user need to know what changes has been made
to the library without the need to look for the diffs inside the libraries files.

There is currently **no standard format** for the change log, but most of the libraries
\*cough cough\* use the following:

The first heading (except the change log heading) is the date when the change was made
in the format `day-month-year`, for example `## 14-04-2024`.

Everything after the heading (and before the next) is divided into categories based on the type of the made change:
- `Added`: new added features
- `Changed`: things that were already working in the previous version but has been modified
- `Fixed`: problems that were discovered in the previous version and has been fixed
- `Bug`: found bug that has not been resolved yet
- `BREAKING`: changes that breaks most of the previous functionalities of the library

The category is also a heading, for example `### Added`.

Inside each category a small description of each change can be found as a unordered list,
each item should describe a single change (that can be either small or large), if multiple
changes are made they should be explained in different rows, for example:

```markdown
### Added

- Added new incredible functionality
- Added another new incredible functionality that has absolutely nothing to do
  with the previous one :)
```

> [!TIP]
> If multiple changes are made on the same day, they can be added to the same
> block even if they are different commits

