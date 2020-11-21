# Shell

## Usage

### Changing the current working directory

The shell supports the `cd` command:

```sh
/a/b/c µ cd d # Change the current directory to d/

/a/b/c/d µ cd .. # Go back to the parent directory

/a/b/c µ
```

But also shorthand navigation:

```sh
/a/b/c µ d # Change the current directory to d/

/a/b/c/d µ .. # Go back to the parent directory

/a/b/c µ .... # Go back to the parent directory but each additional . got one level upper.

/ µ
```
