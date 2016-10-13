#Atto Emacs
The smallest functional Emacs in less than 2000 lines of C.

Atto Emacs is inspired by MicroEmacs, Nano, Pico and my earlier project known as Perfect Emacs [1].

> A designer knows he has achieved perfection not when there is nothing left to add, but when there is nothing left to take away.
> -- <cite>Antoine de Saint-Exupery</cite>

##Goals of Atto Emacs
* Be the smallest fuctional Emacs in less than 2000 lines of C.
* Provide a rich level of functionality in the smallest amount of code
* Be easy to understand without extensive study (to encourage further experimentation).

In Defining Atto as the lowest functional Emacs I have had to consider the essential feature set that makes Emacs, 'Emacs'.  I have defined this point as a basic Emacs command set and key bindings; the ability to edit multiple files (buffers), and switch between them; edit the buffers in mutliple windows, cut, copy and paste; forward and reverse searching and a replace function. The proviso being that all this will fit in less than 2000 lines of C.

As of Atto v1.4, these goal has been achieved !.

##Why the name Atto?
The small Emacs naming scheme appears to use sub-unit prefixes in decending order with each further reduction of functionality.  Atto means 10 to the power of minus 18.   Logically Femto (10^-15) comes after Pico (10^-12). However choosing Atto allows for the potential for Femto to be an Atto based Emacs with a scripting language.

##Derivation
Atto is based on the public domain code of Anthony Howe's editor (commonly known as Anthony's Editor or AE, [2]).  Rather than representing a file as a linked list of lines, the AE Editor uses the concept of a Buffer-Gap [4,5,6].  A Buffer-Gap editor stores the file in a single piece of contiguous memory with some extra unused space known as the buffer gap.  On character insertion and deletion the gap is first moved to the current point.  A character deletion then extends the gap by moving the gap pointer back by 1 OR the gap is reduced by 1 when a character is inserted.  The Buffer-Gap technique is elegant and significantly reduces the amount of code required to load a file, modify it and redraw the display.  The proof of this is seen when you consider that Atto supports almost the same command set that Pico supports,  but Pico requires almost 17 times the amount of code.

## Atto v1.7,  12 October 2016
* Added filename completion
NOTE: If this creates a problem on windows the change can be backed out by uncommenting the line in command.c readfile() and using the getinput() instead of getfilename() functions.

## Atto v1.6,  29 December 2015
* Fixed display problem when editing same buffer in multiple windows.
* Fixed overflow of filename variable when a large filename is supplied at the command line
* Reformatted comments at top of source files to claw back 30 more lines for defect fixes

## Atto v1.5,  20 December 2015
* Added INS = toggle-overwrite-mode
* flushed keyboard on detection of Esc in search and replace, which means if you touch the arrow keys you exit cleanly without writing the rest of an escape sequence into the buffer.
* Added entries for escape key binding (eg esc-v and esc-V) to handle when CAPSLOCK is active.
* Handled CAPSLOCK when prompted y/n to exit.
* Line count is 1987.

## Atto v1.4.3, 15 December 2015
* fixed bug with display of last line

## Atto v1.4.2, 13 December 2015
* Fixed crash bug with free_windows on MS windows, thanks to Ed Davies for reporting
* went through all files and ensure correct indentation and tabstops, ensured if (foo) { opening brace style
* line count is 1955

## Atto v1.4.1, 12 December 2015
* Added esc-@ as alternative to C-space for set-mark

## Atto v1.4 8 December 2015
* Working Atto that supports multiple windows. It all fits in 1969 lines of C !
* Fixed bug that meant that scrolling to end of file in other than the first window jumped off the screen.
* Removed previous-buffer which was not going to get used.
* Removed redundant definitions of FIRST_LINE
* Corrected PgDn and PgUp to work with variable size windows.

## Atto V1.3.1 5 December 2015
* Fixed bug with count_buffers causing kill-buffer to core dump

## Atto V1.3 1 December 2015
* Added M-r search and replace
* Added M-g goto-line
* Updated show-pos with line/total_lines count
* fixed bug introduced in 1.2 where last modified buffer would not trigger the prompt to save.
* code footprint is 1775 lines !

## Atto v1.2 29 November 2015
* Implemented multibuffer support, added approx 180 lines of code
* First buffer created is called *scatch*
* Creates *scratch* buffer if you want to delete last buffer
* Added C-x C-n next-buffer
* Added C-x C-p prev-buffer
* Added C-x k   delete-buffer
* Consumed key.h into header.h
* code footprint is 1586 lines !

## Atto v1.1 26 November 2015
* Reduced code footprint by simplification of key code and the definition of keymap. This means I dont have to edit more than one map to add a function.
* Added forward and reverse text search.
* Code footprint is less than 1400 lines !


##Comparisons with Other Emacs Implementations

    Editor         Binary   BinSize     KLOC  Files

    atto           atto       33002     1.9k     10
    pEmacs         pe         59465     5.7K     16
    Esatz-Emacs    ee         59050     5.7K     14
	GNOME          GNOME      55922     9.8k     13
    Zile           zile      257360    11.7k     48
    Mg             mg        585313    16.5K     50
    uEmacs/Pk      em        147546    17.5K     34
    Pico           pico      438534    24.0k     29
    Nano           nano      192008    24.8K     17
	jove           jove      248824    34.7k     94
	Qemacs         qe        379968    36.9k     59
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
    M-g   goto-line
    M-r   Search and Replace
    M-w   copy-region

    C-<spacebar> Set mark at current position.

    ^X^C  Exit. Any unsaved files will require confirmation.
    ^X^F  Find file; read into a new buffer created from filename.
    ^X^S  Save current buffer to disk, using the buffer's filename as the name of
    ^X^W  Write current buffer to disk. Type in a new filename at the prompt to
    ^Xi   Insert file at point
    ^X=   Show Character at position
    ^X^N  next-buffer
    ^Xn   next-buffer
    ^Xk   kill-buffer
    ^X1   delete-other-windows
    ^X2   split-window
    ^Xo   other-window

    Home  Beginning-of-line
    End   End-of-line
    Del   Delete character under cursor
    Ins   Toggle Overwrite Mode
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

As of Atto 1.6 we have about 46 lines of code before we reach the design limit of 2000 lines.  Whilst I would have liked to have added a few other features the priority will now be bug fixes and keeping the code count below 2000 lines.
       
##Multiple Windows or Not?

Atto supports multiple windows !  This was the hardest part of the project to get working reliably.

The lack of multiple windows would have been quickly noticed as it is a very visible feature of the Emacs user interface.  It is very useful to be able to look at some code in one window whilst editing another section of the same file (or a different file) in another window.  As more than one window can access the same buffer the current point now has now to be associated with the window structure and updated back to the buffer structure whenever any gap or display code is called that accesses the point location. The strategy I used in the end was to treat the buffer as the master and update the window structure with copies of the critical values (point, page, epage, cursor row & col) after each display update of that window.  This is because the display code does the calculations necessary to reframe the sceen when the point scrolls up off the screen or below the screen. Getting everthing to work correctly when displaying the same buffer in more that one winow was a reall challenge and took arpund 15-20 hours to get it working.

A multi-window display issue (specifically evident in a buffer-gap editor) was resolved in Atto 1.6.  This is where you are editing a file that is displayed in more than one window.  Say you are in window1 and delete 3 lines, the current point in the other windows (if the point is below the point in window1) have to be adjusted to take into account that thier relative positions in the buffer have now shifted up. We do this by tracking the size of the text in the buffer before and after each command.  At the start of the display function we can work out the difference and adjust the other windows when they are updated. This mechanism works well even when inserting a text file that means that the gap has to be re-allocated.


##Known Issues
	Goto-line will fail to go to the very last line.  This is a special case that could easily be fixed.

##Copying
  Atto code is released to the public domain.
  hughbarney AT gmail.com 2016

##References
    [1] Perfect Emacs - https://github.com/hughbarney/pEmacs
    [2] Anthony's Editor - https://github.com/hughbarney/Anthony-s-Editor
    [3] MG - https://github.com/rzalamena/mg
    [4] Jonathan Payne, Buffer-Gap: http://ned.rubyforge.org/doc/buffer-gap.txt
    [5] Anthony Howe,  http://ned.rubyforge.org/doc/editor-101.txt
    [6] Anthony Howe, http://ned.rubyforge.org/doc/editor-102.txt

