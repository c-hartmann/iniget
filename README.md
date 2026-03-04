# IniGet / IniSet

 IniGet / IniSet is simple INI File parser to get and set values in such files.
 
 It is also a first time C++ project to learn the language and therefore
 anything but perfect in that sence, but it mostly does the job.
 
 
## What is an INI file?

An INI file is an ASCII file describing simple parameters (character strings, 
integers, floating-point values or booleans) in an explicit format, easy to 
use and modify for users.

An INI file is segmented into *Sections*, declared by the following syntax:

[Section Name]

i.e. the section name enclosed in square brackets, alone on a line. 
Sections names are allowed to contain any character but square brackets or linefeeds.

In any section are zero or more *Variables*, declared with the following syntax:

```
key = value
```

The *Key* is any string (even possibly containing blanks). The *Value* is any character 
on the right side of the equal sign. Values can be given enclosed with quotes. If no 
quotes are present, the value is understood as containing all characters between 
the first and the last non-blank characters before an optional *Comment* declared
with a '#'.

Comment lines may exist on their own:

```
# this is a comment
```


## Usage

Basic and most common usage (with some exceptions) requires a file path, 
a section name and a key name:

```
$ iniget file.ini  'Section' 'Key'
```


## Options

```
```


```
```

## Special Usages

To validate an inifile:

```
$ iniget --validate file.ini
```

Get a key from files without any section by leaving the section name empty - might 
depend on your shell:

```
$ iniget file.ini  '' 'key'
```

as in:

```
$ iniget /etc/os-release '' VERSION_CODENAME
trixie
```

## Arrays

We found a couple of more or less reasonable formats to express array values. As there
is no common representation for all purposes IniGet will leave the interpretation to the
calling instance. A Bash shell example with this key value line:

```
[array_section]
ini_array = one two three four
```

might get read in Bash with:

```
declare -a bash_array
bash_array+=($(iniget inifile.ini  'array_section' 'ini_array'))
```

The same priciple applies to any reasonable form of a boolean value.


## TODO

Multi-line values can be provided by ending the line with a backslash (\).

```
Multiple = Line 1 \
Line 2 \
Line 3 \
Line 4 ; comment
```


## DISCLAIMER

Parts of this text are shameless "borrowed" (from: https://gitlab.com/iniparser/iniparser
