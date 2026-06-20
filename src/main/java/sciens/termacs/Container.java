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

    // P5 selection & input widgets (§5.10)
    public CheckBox    addCheckBox(String text)     { return new CheckBox(app, Native.addCheckBox(app, h, text)); }
    public OptionGroup addOptionGroup(SelectMode m) { return new OptionGroup(app, Native.addOptionGroup(app, h, m.ordinal())); }
    public OptionGroup addRadioGroup()              { return addOptionGroup(SelectMode.ONE); }
    public OptionGroup addCheckGroup()              { return addOptionGroup(SelectMode.MANY); }
    public ComboBox    addComboBox()                { return new ComboBox(app, Native.addComboBox(app, h)); }
    public ProgressBar addProgressBar()             { return new ProgressBar(app, Native.addProgressBar(app, h)); }
    public TextArea    addTextArea()                { return new TextArea(app, Native.addTextArea(app, h)); }
    public Frame       addFrame(String title)       { return new Frame(app, Native.addFrame(app, h, title)); }
    public ScrollView  addScrollView()              { return new ScrollView(app, Native.addScrollView(app, h)); }
}
