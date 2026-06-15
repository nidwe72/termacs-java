package sciens.termacs;

public final class Menu extends Widget {
    Menu(long app, long h) { super(app, h); }
    public void addItem(String label, Runnable onActivate) { Native.menuAddItem(app, h, label, (i, t) -> onActivate.run()); }
}
