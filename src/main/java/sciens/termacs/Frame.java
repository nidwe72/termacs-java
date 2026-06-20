package sciens.termacs;

/** Bordered, titled container (§5.10). Add children via the inherited factories. */
public final class Frame extends Container {
    Frame(long app, long h) { super(app, h); }
    public void setTitle(String t) { Native.frameSetTitle(app, h, t); }
}
