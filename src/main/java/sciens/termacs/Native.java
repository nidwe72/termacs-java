package sciens.termacs;

/**
 * Package-private 1:1 binding to the flat C ABI (termacs.h) via the JNI shim
 * (libtermacsjni.so). Handles are {@code long}s; one {@link Cb} type carries
 * every event (mirrors the variant TmEvent). Public OOP classes wrap this.
 */
final class Native {
    private Native() {}

    /** One callback type for all events/thunks (variant TmEvent: i + text). */
    interface Cb { void invoke(int i, String text); }

    static {
        String explicit = System.getProperty("termacs.jni");
        if (explicit != null) System.load(explicit);
        else System.loadLibrary("termacsjni");
        int v = abiVersion();
        if (v != 1) throw new IllegalStateException("termacs ABI mismatch: native=" + v + " expected=1");
    }

    // ABI
    static native int abiVersion();
    static native int lastStatus();
    static native String statusMessage();

    // application
    static native long appNewHeadless(int w, int h);
    static native long appNewTerminal();
    static native void appFree(long app);
    static native void appSetTheme(long app, int theme);
    static native int  appRun(long app);
    static native void appQuit(long app, int code);
    static native void appPost(long app, Cb fn);
    static native long appStartTimer(long app, int ms, Cb fn);
    static native void appStopTimer(long app, long id);
    static native void appRender(long app);
    static native int  appScreenW(long app);
    static native int  appScreenH(long app);
    static native String appRowText(long app, int y);
    static native void appFeedKey(long app, int key, int ch);

    // windows
    static native long createWindow(long app, String title);
    static native void windowResize(long app, long win, int w, int h);
    static native long windowSetMenubar(long app, long win);
    static native long windowSetStatusbar(long app, long win);
    static native long windowSetContentVbox(long app, long win);
    static native long windowSetContentHbox(long app, long win);
    static native void windowShow(long app, long win);

    // containers
    static native void containerSetPadding(long app, long c, int cells);
    static native void containerSetSpacing(long app, long c, int cells);
    static native long addLabel(long app, long c, String text);
    static native long addButton(long app, long c, String text);
    static native long addLineEdit(long app, long c);
    static native long addListView(long app, long c);
    static native long addHBox(long app, long c);
    static native long addVBox(long app, long c);

    // common
    static native void widgetSetSizing(long app, long w, int axis, int min, int pref, int max, int stretch);
    static native void widgetSetFocus(long app, long w);
    static native void widgetRemove(long app, long w);

    // leaves
    static native void labelSetText(long app, long w, String t);
    static native void buttonSetText(long app, long w, String t);
    static native void lineEditSetText(long app, long w, String t);
    static native String lineEditText(long app, long w);
    static native void listViewAddItem(long app, long w, String t);
    static native void listViewRemoveItem(long app, long w, int row);
    static native int  listViewCount(long app, long w);
    static native int  listViewSelected(long app, long w);
    static native String listViewItemAt(long app, long w, int row);

    // menus & status
    static native long menubarAddMenu(long app, long mb, String title);
    static native void menuAddItem(long app, long menu, String label, Cb fn);
    static native void statusbarSetText(long app, long w, String t);
    static native void statusbarSetRightText(long app, long w, String t);

    // signals
    static native void onClicked(long app, long w, Cb fn);
    static native void onSubmitted(long app, long w, Cb fn);
    static native void onTextChanged(long app, long w, Cb fn);
    static native void onActivated(long app, long w, Cb fn);

    // dialogs
    static native void dialogInfo(long app, long win, String message);
    static native void dialogConfirm(long app, long win, String message, Cb fn);
}
