#!/bin/bash

printHelp ()
{
    echo "###################### Commands ######################"
    echo "Set environment variable JDRF_CLANG_FORMAT to the desired clang-format executable"
    echo "### -r, --recursive -> Run recursively in specified folder"
    echo "### -g, --git-diff-master -> Run only on files that have changed since origin/master"
    echo "### -i, --inplace -> Edit file inplace (overwrite existing file)"
    exit 1
}
GIT_ROOT_DIR=$(git rev-parse --show-toplevel)/development
RECURSIVE="0"
INPLACE="0"
EXCLUDE="0"
EXCLUDE_FILE_PATH="${GIT_ROOT_DIR}/.clang-format-exclude"
RUN_ON_CHANGED_GIT_FILES="0"
FORMAT_PATHS=()
FORMAT_FILES=()
CLANG_FORMAT_EXE=""

##################################################################
# Input Argument Parsing
##################################################################
for ((i=1; i<=$#; i++)) do
    arg=${!i}
    case "$arg" in
        -h|--help )
            printHelp
            ;;
        -r|--recursive )
            RECURSIVE="1"
            ;;
        -g|--git-diff-master )
            RUN_ON_CHANGED_GIT_FILES="1"
            ;;
        -i|--inplace )
            INPLACE="1"
            ;;
        -e|--exclude )
            EXCLUDE="1"
            ;;
        *)
            if [ -d "${arg}" ]; then
                # append path to paths array
                FORMAT_PATHS+=( $(realpath "${arg}") )
            elif [ -f "${arg}" ]; then
                # append file to files array
                FORMAT_FILES+=( $(realpath "${arg}") )
            else
                echo "Unknown input \"$arg\""
                printHelp
                exit 1
                fi
            ;;
    esac
done

##################################################################
# Make string of exclude paths
##################################################################
getBlackListFindExlusion()
{
    local blackListPath=${1}
    # return expected:
    # \( -path [path1] -o -path [path2] \)
    local findExclusionString="! \( "

    local isFirstLine="1"
    # Read rest of file and add as exclusion paths
    while IFS= read -r line; do
        path="${line}"

        # If first line in file, don't prepend -o
        if [ "1" == "${isFirstLine}" ]; then
            findExclusionString="${findExclusionString} -path '${line}'"
            isFirstLine="0"
        else
            findExclusionString="${findExclusionString} -o -path '${line}'"
        fi
    done < ${blackListPath}
    echo "${findExclusionString} \)"
}

##################################################################
# Format Single File
##################################################################
formatFile()
{
    local filePath=${1}
    local overwrite=${INPLACE}
    local fileExtension=${filePath##*.}

    # Should only format C files
    if [ "c" == "${fileExtension}" -o "h" == "${fileExtension}" ]; then

        # Make a file path named [filePath]_formatted.[extension]
        local formattedFilePath="${1}.clangformat"

        # Format file and write to formatted file path
        ${CLANG_FORMAT_EXE} -style=file -fallback-style=none ${filePath} > ${formattedFilePath}

        # Get number of lines modified by format
        local diffCnt=$(diff ${filePath} ${formattedFilePath} | grep -i '^>' | wc -l)

        # If inplace flag is provided, overwrite original file with formatted file
        if [ "1" == "${overwrite}" ]; then
            rm "${filePath}"
            mv "${formattedFilePath}" "${filePath}"
        # Otherwise, delete formatted file
        else
            rm "${formattedFilePath}"
        fi
        echo "${diffCnt} changes for: ${filePath}"
    else
        echo "${filePath} is not a .c or .h file!"
    fi

}

##################################################################
# Recursively Format Directory
##################################################################
formatRecursiveDir()
{
    local dirPath=${1}
    echo "Formatting Recursively in ${dirPath}"

    local findParams=""
    if [ "1" == "${EXCLUDE}" ]; then

        # Find all files that end in *.c or *.h in current dir other than in paths excluded
        # find command:
        # find [dir] -type f ! \( -path [path1] -o -path [path2] \) -a \( -iname "*.h" -o  -iname "*.c" \) -print
        findParams="${dirPath} -type f $(getBlackListFindExlusion ${EXCLUDE_FILE_PATH}) -a \( -iname \"*.h\" -o -iname \"*.c\" \) -print"
    else
        # Find all files that end in *.c or *.h in current dir
        # find command:
        # find [dir] -type f  \( -iname "*.h" -o  -iname "*.c" \) -print
        findParams="${dirPath} -type f \( -iname \"*.h\" -o -iname \"*.c\" \) -print"
    fi

    local findCommand="find ${findParams}"
    local findResults=$(eval "${findCommand}")

    # Split results by newline and run formatter on each file
    for file in ${findResults/"\n"/ };
    do
        formatFile $(realpath "${file}")
    done
}

##################################################################
# Format Directory
##################################################################
formatDir()
{
    local dirPath=${1}
    echo "Formatting in ${dirPath}"

    local lsResults=$(eval "ls ${dirPath}/*.c ${dirPath}/*.h 2> /dev/null")

    # Split results by newline and run formatter on each file
    for file in ${lsResults/"\n"/ };
    do
        formatFile $(realpath "${file}")
    done
}

##################################################################
# Format all Files changed since origin master
##################################################################
formatChangedGitFiles()
{
    # This gets the first commit that is an ancestor of both the current branch and origin/master
    # A - B - F - G - H - I
    #      \
    #       C - D - E
    # If our current branch is E and we compare to origin/master (I) there will be many changes
    # that we don't want the linter to run on. This command will return B, so we can run the linter
    # on only the changes that have been made to this branch.
    local branchBase=$(diff -u <(git rev-list --first-parent HEAD) <(git rev-list --first-parent origin/master) | sed -ne 's/^ //p' | head -1)

    echo "Formatting all files changed since ${branchBase}"

    # Get list of files that have changed since master
    local diffFilesList=$(git diff ${branchBase} --name-only)

    # Split results by newline and run formatter on each file
    for file in ${diffFilesList/"\n"/ };
    do
        formatFile $(realpath "${GIT_ROOT_DIR}/${file}")
    done
}

##################################################################
# Verify ClangFormat version
##################################################################
assertMinimumClangFormatVersion()
{
    local clangFormatVersion=$(${CLANG_FORMAT_EXE} --version)
    local requiredVersion="9"
    local regex='clang-format version ([0-9]+)\.([0-9]+)\.([0-9]+).+'

    if [[ $clangFormatVersion =~ $regex ]];
    then
        clangFormatVersion="${BASH_REMATCH[1]}"
    fi

    if [ "${requiredVersion}" -gt "${clangFormatVersion}" ]; then
        echo "Minimum ClangFormat version 9.0.0 is required"
        exit 0
    fi
}

##################################################################
# Set ClangFormat exe
##################################################################
getClangFormatExe()
{
    if [ "0" != "${#JDRF_CLANG_FORMAT}" ];
    then
        CLANG_FORMAT_EXE="${JDRF_CLANG_FORMAT}"
    else
        CLANG_FORMAT_EXE="clang-format"
    fi
    assertMinimumClangFormatVersion
}

getClangFormatExe

pushd ${GIT_ROOT_DIR} > /dev/null


if [ "1"  == "$RUN_ON_CHANGED_GIT_FILES" ]; then
    formatChangedGitFiles
fi

for path in "${FORMAT_PATHS[@]}"
do
    if [ "1" == "${RECURSIVE}" ]; then
        formatRecursiveDir ${path}
    else
        formatDir ${path}
    fi
done

for file in "${FORMAT_FILES[@]}"
do
    formatFile "${file}"
done

popd

echo "Formatter Run"
