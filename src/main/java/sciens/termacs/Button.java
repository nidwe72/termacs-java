package sciens.termacs;

public final class Button extends Widget {
    Button(long app, long h) { super(app, h); }
    public void setText(String t) { Native.buttonSetText(app, h, t); }
    public void onClicked(Runnable r) { Native.onClicked(app, h, (i, t) -> r.run()); }
}
