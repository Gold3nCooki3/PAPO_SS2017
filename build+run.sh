#!/bin/bash
SRC="project/src"
BIN="project/bin"

javac -d $BIN -g $SRC/stuff/*.java $SRC/window/*.java $SRC/main/Main.java
cd project/bin
java main.Main
cd ../..
