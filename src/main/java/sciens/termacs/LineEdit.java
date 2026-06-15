package sciens.termacs;

import java.util.function.Consumer;

public final class LineEdit extends Widget {
    LineEdit(long app, long h) { super(app, h); }
    public void   setText(String t) { Native.lineEditSetText(app, h, t); }
    public String text()            { return Native.lineEditText(app, h); }
    public void   onSubmitted(Runnable r)        { Native.onSubmitted(app, h, (i, t) -> r.run()); }
    public void   onTextChanged(Consumer<String> c) { Native.onTextChanged(app, h, (i, t) -> c.accept(t)); }
}
