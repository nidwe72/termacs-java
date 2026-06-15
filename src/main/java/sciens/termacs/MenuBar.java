package sciens.termacs;

public final class MenuBar extends Widget {
    MenuBar(long app, long h) { super(app, h); }
    public Menu addMenu(String title) { return new Menu(app, Native.menubarAddMenu(app, h, title)); }
}
