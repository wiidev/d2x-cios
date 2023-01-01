#!/bin/bash

OLD_DIR=`pwd`

# Set clean var, removes build and dist, and calls make_modules
make_clean() {
        export CLEAN=clean
        [ -d "build" ] && rm -rf build
        [ -d "dist" ] && rm -rf dist
        make_modules
}

# Replace variables on certain files
replace_vars() {
        echo -n Replacing variables in $1...
        awk -v major_ver=${MAJOR_VERSION} -v minor_ver=${MINOR_VERSION} -f replace.awk "data/$1" > "$2/$1"
        echo done
}

# Build or clean library, plugins and modules
make_modules() {
        do_make cios-lib     ""                ${CLEAN}
        do_make dip-plugin   ${D2XBUILD}/DIPP  ${CLEAN}
        do_make ehci-module  ${D2XBUILD}/EHCI  ${CLEAN}
        do_make es-plugin    ${D2XBUILD}/ES    ${CLEAN}
        do_make fat-plugin   ${D2XBUILD}/FAT   ${CLEAN}
        do_make ffs-plugin   ${D2XBUILD}/FFSP  ${CLEAN}
        do_make mload-module ${D2XBUILD}/MLOAD ${CLEAN}
        do_make sdhc-module  ${D2XBUILD}/SDHC  ${CLEAN}
        do_make usb-module   ${D2XBUILD}/USBS  ${CLEAN}

        [ "${CLEAN}" == "clean" ] && completed

        # Replace variables in some files
        replace_vars ciosmaps.xml      build
        replace_vars ciosmaps-vWii.xml build
        replace_vars d2x-beta.bat      build/${D2XBUILD}
        replace_vars ReadMe.txt        build


        # Copy Changelog.txt to the build directory
        cp data/Changelog.txt build

        [ -z "${DIST}" ] && completed

        echo
        echo -n Creating distribution package...

        # Copy files to ModMii
        [ -d "dist/Support/d2x-beta" ] && rm -rf dist/Support/d2x-beta
        mkdir -p dist/Support/d2x-beta
        cp build/${D2XBUILD}/*     dist/Support/d2x-beta
        cp build/ciosmaps.xml      dist/Support/d2x-beta
        cp build/ciosmaps-vWii.xml dist/Support/d2x-beta
        cp build/ReadMe.txt        dist/Support/d2x-beta
        cp build/Changelog.txt     dist/Support/d2x-beta

        echo done
        echo
        echo "The files are copied to the directory dist/Support/d2x-beta. From there"
        echo "you need to copy/move the Support directory into the ModMii directory."
        echo
        echo "You need to invoke ModMii yourself, on a Windows installation :|"
        echo
}

do_make() {
        echo
        echo Making $1 $3...
        cd source/$1
        make $3 STRIP=../stripios
        if [ $? != 0 ]; then
                echo
                echo Build failed!!!
                quit
        fi
        if [ "$3" != "clean" ]; then

                if [ ! -z "$2" ]; then
                        cp $1.elf ../../build/$2.app
                fi
        fi
        cd - > /dev/null
}

quit() {
        cd ${OLD_DIR}
        exit 1
}

completed() {
        echo
        echo Done!
        echo
        exit 0
}

menu() {
        echo
        echo "Usage 1: $0 [<major_version> [<minor_version> [dist | DIST]]]"
        echo "  It builds d2x with the specified major and minor version."
        echo "  Default values are \"999\" and \"unknown\" respectively."
        echo "  If option dist or DIST is specified then a zip file is generated, i.e. the"
        echo "  distribution package. Be aware that:"
        echo "    - it may take several minutes"
        echo "    - ModMii is required --> http://gbatemp.net/topic/207126-modmii-for-windows"
        echo "    - the MODMII environment variable must be set to ModMii install directory"
        echo "    - internet connection is required"
        echo "  Contrary to DIST, the dist option removes the generated files from ModMii,"
        echo "  allowing you to keep it clean."
        echo "  Examples:"
        echo "   "$0
        echo "   "$0 1
        echo "   "$0 1 final

        echo
        echo "Usage 2: "$0 clean
        echo "  It permanently deletes any previous build and dist."
        echo
        exit 0
}

[ "$1" == "/?" ] && menu
[ "$1" == "-h" ] && menu
[ "$1" == "clean" ] && make_clean

# Set default values
export CLEAN=""
export MAJOR_VERSION=$1
[ -z "${MAJOR_VERSION}" ] && export MAJOR_VERSION="999"
export MINOR_VERSION=$2
[ -z "${MINOR_VERSION}" ] && export MINOR_VERSION="unknown"
export DIST=$3

# Check arguments
if [[ "${MAJOR_VERSION}" =~ [^0-9] ]]; then
        echo "ERROR: The argument <major_version> must be a number"
        menu
fi
if [ "${MAJOR_VERSION}" -gt 999 ]; then
        echo "ERROR: The argument <major_version> can't be greater than 999"
        menu
fi
if [ ${#MINOR_VERSION} -gt 15 ]; then
        echo "ERROR: The argument <minor_version> can\'t be longer than 15 characters"
        menu
fi
if [ ! -z "${DIST}" ] && [ "${DIST}" != "dist" ]; then
        echo ERROR: The 3rd argument must be \"dist\" or empty
        menu

fi

export D2XBUILD=d2x-v${MAJOR_VERSION}-${MINOR_VERSION}

echo -----------------------------
echo Building ${D2XBUILD}
echo -----------------------------

[ ! -d "build/${D2XBUILD}" ] && mkdir -p "build/${D2XBUILD}"
export SOURCE_DATE_EPOCH=$(git log -1 --pretty=%ct)
make_modules
