# KingsFieldRE
King's Field (II) Reverse Engineering Tools and Documentation

This repository contains tools and documentation for reverse engineering King's Field (II).

This is the fruit of my efforts in decompiling and understanding how the game works and is here in the hopes that it might provide help to those who are interested in helping reverse engineer the game.

The tools are available in the repository, and documentation is available in the wiki. As of writing, the wiki is incomplete and new pieces of documentation will be added as soon as possible.

# Tools

All the tools in KingsFieldRE are written using C++11 and the Qt Framework.

## fdat28_tool
This is a graphical tool for interpreting the data contained in FDAT.T's (true) file 28. This file contains data such as weapon stats for each weapon in the game, data for each of the magic spells and the player stats for each achievable level.

![fdat28_tool screenshot](wiki/fdat28_tool.png)

## HexOffsetFinder
This tool is a simple single-purpose utility. It receives text files with increasingly ordered hexadecimal values (such as the ones in the Notes folder), finds the offsets between the values and prints out a sorted list of them. This tool is used to find the size of newly found structures.

## tfile_tool
This is a command line tool for extracting data files from the King's Field (II) CD. It has the ability to give files descriptive names when they have been identified and added to it's internal table.

# Credit where it's due
tfile_tool is heavily based on the TheStolenBattenberg's [Psycpros](https://github.com/TheStolenBattenberg/Psycpros) project.

Monster Mesh IDs were mostly found in [Shrine Maiden's King's Field II code page](http://mikosans.web.fc2.com/code/kings-field-2.html)