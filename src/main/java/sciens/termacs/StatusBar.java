package sciens.termacs;

public final class StatusBar extends Widget {
    StatusBar(long app, long h) { super(app, h); }
    public void setText(String left)       { Native.statusbarSetText(app, h, left); }
    public void setRightText(String right) { Native.statusbarSetRightText(app, h, right); }
}
