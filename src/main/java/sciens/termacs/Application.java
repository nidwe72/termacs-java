package sciens.termacs;

/** Owns the native Application. Use {@link #headless} for tests, the no-arg
 * constructor for a real terminal. */
public final class Application implements AutoCloseable {
    private final long ptr;
    private final Dialogs dialogs = new Dialogs(this);

    private Application(long ptr) { this.ptr = ptr; }

    /** Validate a native app handle; surface the C-ABI error message (e.g. the
     *  terminfo guard, §15.2) instead of a null handle that crashes later. */
    private static long checked(long ptr) {
        if (ptr == 0) {
            String msg = Native.statusMessage();
            throw new IllegalStateException(
                (msg == null || msg.isEmpty()) ? "termacs: failed to create native application" : msg);
        }
        return ptr;
    }

    /** Real terminal (tvision backend). */
    public Application() { this(checked(Native.appNewTerminal())); }
    /** In-memory backend for headless tests / snapshots (§11.10). */
    public static Application headless(int w, int h) { return new Application(checked(Native.appNewHeadless(w, h))); }

    long ptr() { return ptr; }

    public void   setTheme(Theme t)        { Native.appSetTheme(ptr, t.ordinal()); }
    public Window createWindow(String t)   { return new Window(ptr, Native.createWindow(ptr, t)); }
    public int    run()                    { return Native.appRun(ptr); }
    public void   quit(int code)           { Native.appQuit(ptr, code); }

    /** The only thread-safe entry point (§11.6). */
    public void   postToUiThread(Runnable r)        { Native.appPost(ptr, (i, t) -> r.run()); }
    public long   startTimer(int ms, Runnable r)    { return Native.appStartTimer(ptr, ms, (i, t) -> r.run()); }
    public void   stopTimer(long id)                { Native.appStopTimer(ptr, id); }

    public Dialogs dialogs() { return dialogs; }

    // ---- headless test helpers ----
    public void   render()           { Native.appRender(ptr); }
    public String rowText(int y)     { return Native.appRowText(ptr, y); }
    public int    screenWidth()      { return Native.appScreenW(ptr); }
    public int    screenHeight()     { return Native.appScreenH(ptr); }
    public void   feed(Key k)        { Native.appFeedKey(ptr, k.ordinal(), 0); }
    public void   feedChar(char c)   { Native.appFeedKey(ptr, Key.CHAR.ordinal(), c); }

    @Override public void close() { Native.appFree(ptr); }
}
