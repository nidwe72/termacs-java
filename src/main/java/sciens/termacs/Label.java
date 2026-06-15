package sciens.termacs;

public final class Label extends Widget {
    Label(long app, long h) { super(app, h); }
    public void setText(String t) { Native.labelSetText(app, h, t); }
}
