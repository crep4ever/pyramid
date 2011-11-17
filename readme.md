cmake >= 2.6.2
qt >= 4.2

compilation de la lib-tiff:

cd tile/src/lib-ImTiff/tiff-4.0alpha
./configure
make && sudo make install

compilation du programme:

cd tile/
make -> compilation en mode debug
make release -> compilation en mode release

exemples:

./release/noqt  --> sans ihm et sans charger la couche qt
./release/qTopoMap2D --> avec ihmQT
./release/qTopoMap2D ./qscripts/iwcia-03.qs --> lancement via scripts


attention:
 - images au format tif uniquement
 - les fichiers svg générés (exportSvg) sont rapidement trop volumineux pour inkscape