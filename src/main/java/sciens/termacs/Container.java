package sciens.termacs;

/** Box container (VBox/HBox). Factory methods create + parent in one core-owned step. */
public class Container extends Widget {
    Container(long app, long h) { super(app, h); }

    public void setPadding(int cells) { Native.containerSetPadding(app, h, cells); }
    public void setSpacing(int cells) { Native.containerSetSpacing(app, h, cells); }

    public Label    addLabel(String text)  { return new Label(app, Native.addLabel(app, h, text)); }
    public Button   addButton(String text) { return new Button(app, Native.addButton(app, h, text)); }
    public LineEdit addLineEdit()          { return new LineEdit(app, Native.addLineEdit(app, h)); }
    public ListView addListView()          { return new ListView(app, Native.addListView(app, h)); }
    public HBox     addHBox()              { return new HBox(app, Native.addHBox(app, h)); }
    public VBox     addVBox()              { return new VBox(app, Native.addVBox(app, h)); }
}
