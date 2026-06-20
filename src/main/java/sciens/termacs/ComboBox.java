package sciens.termacs;

import java.util.function.IntConsumer;

/** Collapsed single-select dropdown (§5.10). */
public final class ComboBox extends Widget {
    ComboBox(long app, long h) { super(app, h); }
    public void   setOptions(String... opts)       { Native.comboBoxSetOptions(app, h, String.join("\n", opts)); }
    public void   setSelectedIndex(int i)          { Native.comboBoxSetSelectedIndex(app, h, i); }
    public int    selectedIndex()                  { return Native.comboBoxSelectedIndex(app, h); }
    public String selectedText()                   { return Native.comboBoxSelectedText(app, h); }
    public void   setPlaceholder(String p)         { Native.comboBoxSetPlaceholder(app, h, p); }
    public void   onSelectionChanged(IntConsumer c) { Native.onComboChanged(app, h, (i, t) -> c.accept(i)); }
}
