echo "Removing old documentation"
del html /q
doxygen Midi2lr.Doxyfile
cd html
index.html