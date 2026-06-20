package sciens.termacs;

import java.util.function.Consumer;

/** Multi-line text editor (§5.10). */
public final class TextArea extends Widget {
    TextArea(long app, long h) { super(app, h); }
    public void   setText(String t)        { Native.textAreaSetText(app, h, t); }
    public String text()                   { return Native.textAreaText(app, h); }
    public void   appendLine(String t)     { Native.textAreaAppendLine(app, h, t); }
    public void   setReadOnly(boolean r)   { Native.textAreaSetReadOnly(app, h, r); }
    public void   setWordWrap(boolean w)   { Native.textAreaSetWordWrap(app, h, w); }
    public void   setPlaceholder(String p) { Native.textAreaSetPlaceholder(app, h, p); }
    public void   onTextChanged(Consumer<String> c) { Native.onTextAreaChanged(app, h, (i, t) -> c.accept(t)); }
    // §5.11 editing surface
    public void   selectAll()       { Native.textAreaSelectAll(app, h); }
    public String selectedText()    { return Native.textAreaSelectedText(app, h); }
    public void   copy()            { Native.textAreaCopy(app, h); }
    public void   cut()             { Native.textAreaCut(app, h); }
    public void   paste()           { Native.textAreaPaste(app, h); }
    public void   onSelectionChanged(Runnable r) { Native.onTextAreaSelectionChanged(app, h, (i, t) -> r.run()); }
}
