echo "Removing old documentation"
del html /q
del rtf /q
doxygen Midi2lr.Doxyfile
cd html
index.html