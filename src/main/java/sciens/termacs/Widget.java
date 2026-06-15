package sciens.termacs;

/** Base handle. Non-owning; never frees the core node (§4). */
public class Widget {
    final long app;
    final long h;
    Widget(long app, long h) { this.app = app; this.h = h; }

    public long handle() { return h; }
    public void setSizing(Axis a, Sizing s) { Native.widgetSetSizing(app, h, a.ordinal(), s.min, s.preferred, s.max, s.stretch); }
    public void setFocus() { Native.widgetSetFocus(app, h); }
    public void remove()   { Native.widgetRemove(app, h); }
}
