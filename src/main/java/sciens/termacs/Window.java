package sciens.termacs;

public final class Window extends Widget {
    Window(long app, long h) { super(app, h); }
    public void      resize(int w, int height) { Native.windowResize(app, h, w, height); }
    public MenuBar   setMenuBar()      { return new MenuBar(app, Native.windowSetMenubar(app, h)); }
    public StatusBar setStatusBar()    { return new StatusBar(app, Native.windowSetStatusbar(app, h)); }
    public VBox      setContentVBox()  { return new VBox(app, Native.windowSetContentVbox(app, h)); }
    public HBox      setContentHBox()  { return new HBox(app, Native.windowSetContentHbox(app, h)); }
    public void      show()            { Native.windowShow(app, h); }
}
