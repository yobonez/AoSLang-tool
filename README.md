# AoSLang-tool

Tool for AoSLang-XX.bin files from Ben Aksoy's Ace of Spades voxlap game.

You can read, export and pack them with it.

## Usage
`aoslang-tool.exe  <export|read|pack> <filename> [out filename]`

`<> - required, [] - optional - for "export" & "pack"`

## Usage examples

`aoslang-tool.exe read AoSLang-EN.bin` 
Reads the file and outputs strings and its respective offsets.

`aoslang-tool.exe export AoSLang-EN.bin MyAoSLangExport.txt` 
Exports all the strings to text file.

`aoslang-tool.exe pack MyAoSLangExport.txt MyAosLangFile.bin` 
Takes text file with strigns and converts it to aoslang binary file.