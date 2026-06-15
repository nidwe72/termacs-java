package sciens.termacs;

/** Normalized key (ordinal MUST match TmKey in the C ABI). */
public enum Key {
    NONE, CHAR,
    ENTER, ESCAPE, TAB, BACKTAB, BACKSPACE, DELETE, INSERT,
    UP, DOWN, LEFT, RIGHT, HOME, END, PAGEUP, PAGEDOWN,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
}
