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

The `src` folder contains the source of the library and `inc` the headers. \
In the `README.md` information about usage and development of the library can be found.

The `test` folders contains files used for [unit testing](https://it.wikipedia.org/wiki/Unit_testing)
with the [Unity Test Framework](http://www.throwtheswitch.org/unity). \
These folder is usually only needed for development of the library, but you can test it by running `make test_all`.

## Documentation

Check out the [docs](docs) folder for the documentation of each library

## Usage

If you need to use one or more libraries from this repo, include it as a submodule
to keep it updated to the latest version.

```shell
git submodule add git@github.com:eagletrt/micro-libs.git
```

> [!NOTE]
> Avoid to compile and link all the files contained in this repo to your project.
> Use only the libraries that you need.

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

Remember to **update** the `README.md` and the documentation every time you make important changes to the library.

Every time a new feature is added or an old one is removed in a file, **update**
the corresponding test file in the `test` folder. \
If you make any changes to the library, even small ones, **run** `make test_all` to ensure
that everything works correctly, if at least one test fails or the program crash,
**solve** the problem and re-run the test until each one is passed.

