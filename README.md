#Atto Emacs
The smallest fuctional Emacs in less than 2000 lines of C.

Atto Emacs is inspired by MicroEmacs, Nano, Pico and my earlier project known as Perfect Emacs [1].

> A designer knows he has achieved perfection not when there is nothing left to add, but when there is nothing left to take away.
> -- <cite>Antoine de Saint-Exupery</cite>

##Goals of Atto Emacs
* Be the smallest fuctional Emacs in less than 2000 lines of C.
* Provide a rich level of functionality in the smallest amount of code
* Be easy to understand without extensive study (to encourage further experimentation).

In Defining Atto as the lowest functional Emacs I have had to consider the essential feature set that makes Emacs, 'Emacs'.  I have defined this point as a basic Emacs command set plus the ability to edit multiple files (buffers) and (possibly) their display in multiple windows.  The proviso being that all this will fit in less than 2000 lines of C.

As of Atto v1.0.0, these goal has been almost been achieved. So far we have a basic command set working on a single file in a single buffer.  However it will be a small matter to make the code generic to operate on multiple instances of buffers.  This will be a priority in the next release.

##Why the name Atto?
The small Emacs naming scheme appears to use sub-unit prefixes in decending order with each further reduction of functionality.  Atto means 10 to the power of minus 18.   Femto (10^-15) comes after Pico (10^-12). However choosing Atto allows for the potential for Femto to be an Atto based Emacs with a scripting language.

##Derivation
Atto is based on the public domain code of Anthony Howe's editor (commonly known as Anthony's Editor or AE, [2]).  Rather than representing a file as a linked list of lines, the AE Editor uses the concept of a Buffer-Gap [4,5,6].  A Buffer-Gap editor stores the file in a single piece of contiguous memory with some extra unused space known as the buffer gap.  On character insertion and deletion the gap is first moved to the current point.  A character deletion then extends the gap by moving the gap pointer back by 1 OR the gap is reduced by 1 when a character is inserted.  The Buffer-Gap technique is elegant and significantly reduces the amount of code required to load a file, modify it and redraw the display.  The proof of this is seen when you consider that Atto supports almost the same command set that Pico supports,  but Pico requires almost 17 times the amount of code.


## Atto v1.1 26 November 2015
* Reduced code footprint by simplification of key code and the definition of keymap. This means I dont have to edit more than one map to add a function.
* Added forward and reverse text search.
* Code footprint is less than 1400 lines !


##Comparisons with Other Emacs Implementations

    Editor         Binary   BinSize     KLOC  Files

    atto           atto       33002     1.4k      9
    pEmacs         pe         59465     5.7K     16
    Esatz-Emacs    ee         59050     5.7K     14
	GNOME          GNOME      55922     9.8k     13
    Zile           zile      257360    11.7k     48
    Mg             mg        585313    16.5K     50
    uEmacs/Pk      em        147546    17.5K     34
    Pico           pico      438534    24.0k     29
    Nano           nano      192008    24.8K     17
	jove           jove      248824    34.7k     94
    ue3.10         uemacs    171664    52.4K     16
    GNUEmacs       emacs   14632920   358.0k    186

##Atto Key Bindings
    C-A   begining-of-line
    C-B   backward-character
    C-D   delete-char
    C-E   end-of-line
    C-F   forward Character
	C-G	  Abort (at prompts)
    C-H   backspace
	C-I   handle-tab
    C-J   newline
    C-K   kill-to-eol
    C-L   refresh display
    C-M   Carrage Return
    C-N   next line
    C-P   previous line
	C-R   search-backwards
	C-S	  search-forwards
    C-U   Undo
    C-V   Page Down
	C-W   Kill Region (Cut)
    C-X   CTRL-X command prefix
    C-Y   Yank (Paste)

    M-<   Start of file
    M->   End of file
    M-v   Page Up
    M-f   Forward Word
    M-b   Backwards Word
    M-w   copy-region

    C-<spacebar> Set mark at current position.

    ^X^C  Exit. Any unsaved files will require confirmation.
    ^X^F  Find file; read into a new buffer created from filename.
    ^X^S  Save current buffer to disk, using the buffer's filename as the name of
    ^X^W  Write current buffer to disk. Type in a new filename at the prompt to
    ^Xi   Insert file at point
    ^X=   Show Character at position

    Home  Beginning-of-line
    End   End-of-line
    Del   Delete character under cursor
    Left  Move left
    Right Move point right
    Up    Move to the previous line
    Down  Move to the next line
    Backspace delete caharacter on the left
    Ctrl+Up      beginning of file
    Ctrl+Down    end of file
    Ctrk+Left    Page Down
    Ctrl+Right   Page Up

###Copying and moving
    C-<spacebar> Set mark at current position
    ^W   Delete region
    ^Y   Yank back kill buffer at cursor
    M-w  Copy Region

A region is defined as the area between this mark and the current cursor position. The kill buffer is the text which has been most recently deleted or copied.

Generally, the procedure for copying or moving text is:
1. Mark out region using M-<spacebar> at the beginning and move the cursor to the end.
2. Delete it (with ^W) or copy it (with M-W) into the kill buffer.
3. Move the cursor to the desired location and yank it back (with ^Y).

###Searching
    C-S or C-R enters the search prompt, where you type the search string
    BACKSPACE - will reduce the search string, any other character will extend it
    C-S at the search prompt will search forward, will wrap at end of the buffer
    C-R at the search prompt will search backwards, will wrap at start of the buffer
    ESC will escape from the search prompt and return to the point of the match
    C-G abort the search and return to point before the search started

##Building on Ubuntu

When building on Ubuntu you will need to install the libcurses dev package.

$ sudo apt-get install apt-file
$ apt-file update

now search for which package would have curses.h
$ apt-file search curses.h

libncurses5-dev: /usr/include/curses.h

$ sudo apt-get install libncurses5-dev


##Future Enhancements
Atto currently only works on one file, one buffer, one window.  This will be a priority of the next release.   The ability to edit multiple files (single window view) could be added by modifying the code to encapsulate the buffer variables into a structure. It is expected this would add between 50-100 lines of code.  At least two additional key bindings will be required to manage multiple buffers.

    ^XN   Next Buffer
    ^X^B  Show buffer menu in a window

The following key strokes could be added fairly cheaply

    ^X(   Start recording a keyboard macro.
    ^X)   Stop recording macro.
    ^XE   Execute macro.

It would be simple to add the following key-bindings

    Esc-Left     Delete word left
    Esc-right    Delete word right
	Esc-g        Goto Line

##Known Issues
   The arrow keys dont work properly when entering filenames on the prompt line

##Copying
  Atto code is released to the public domain.
  hughbarney@gmail.com November 2015

##References
    [1] Perfect Emacs - https://github.com/hughbarney/pEmacs
    [2] Anthony's Editor - https://github.com/hughbarney/Anthony-s-Editor
    [3] MG - https://github.com/rzalamena/mg
    [4] Jonathan Payne, Buffer-Gap: http://ned.rubyforge.org/doc/buffer-gap.txt
    [5] Anthony Howe,  http://ned.rubyforge.org/doc/editor-101.txt
    [6] Anthony Howe, http://ned.rubyforge.org/doc/editor-102.txt

