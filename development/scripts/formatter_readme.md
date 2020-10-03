# Linter.sh Instructions
To run the linter run: `./scripts/linter.sh [options]` from the `development` root directory.

# Options

## Recursive
### Flag
`-r` or `--recursive`
### Description
Run linter recursively in all specified directories.

## Git Diff Master
### Flag
`-g` or `--git-diff-master`
### Description
Run linter on files that have been changed since origin/master.

## Inplace
### Flag
`-i` or `--inplace`
### Description
Edit files inplace - overwrite existing files with suggested changes.

## Exclude
### Flag
`-e` or `--exclude`
### Description
Exclude files and directories specified in `thor/.clang-format-exclude` when running recursively.

# Examples

`./scripts/linter.sh -r . -e`  - Run linter recursively in the whole repo, excluding files and directories specified in `thor/.clang-format-exclude`

`./scripts/linter.sh -g`  - Run linter on files that have changed since origin/master

`./scripts/linter.sh -i [someDir] [someOtherDir] [someFile]`  - Run linter inplace on .c and .h files in `[someDir]` and `[someOtherDir]` and on '[someFile]'

