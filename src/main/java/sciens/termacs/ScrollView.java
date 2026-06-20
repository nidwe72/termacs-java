package sciens.termacs;

/** Clipping viewport that scrolls a (taller) content child (§5.10). */
public final class ScrollView extends Container {
    ScrollView(long app, long h) { super(app, h); }
    public void scrollTo(int x, int y) { Native.scrollViewScrollTo(app, h, x, y); }
}
