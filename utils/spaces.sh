#!/bin/sh
#Description: remove trailing spaces

sed -i 's/[ \t]*$//' src/ihm/*.*
sed -i 's/[ \t]*$//' src/lib-mapkernel/pyramid/*.*
sed -i 's/[ \t]*$//' src/lib-utils/*.*
sed -i 's/[ \t]*$//' src/lib-clustering/*.*
sed -i 's/[ \t]*$//' src/lib-image2D/*.*
sed -i 's/[ \t]*$//' cmake/modules/*.*