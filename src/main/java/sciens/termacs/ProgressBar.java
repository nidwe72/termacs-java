package sciens.termacs;

/** Determinate progress bar 0..100 (§5.10). */
public final class ProgressBar extends Widget {
    ProgressBar(long app, long h) { super(app, h); }
    public void setValue(int percent) { Native.progressBarSetValue(app, h, percent); }
    public int  value()               { return Native.progressBarValue(app, h); }
}
