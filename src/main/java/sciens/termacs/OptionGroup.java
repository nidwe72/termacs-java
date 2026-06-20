package sciens.termacs;

import java.util.function.IntConsumer;

/** Single (radio) and multiple (check) selection group (§5.10). */
public final class OptionGroup extends Widget {
    OptionGroup(long app, long h) { super(app, h); }
    public void    setOptions(String... opts)        { Native.optionGroupSetOptions(app, h, String.join("\n", opts)); }
    public void    setOrientation(Axis a)            { Native.optionGroupSetOrientation(app, h, a.ordinal()); }
    public void    setSelectedIndex(int i)           { Native.optionGroupSetSelectedIndex(app, h, i); }
    public int     selectedIndex()                   { return Native.optionGroupSelectedIndex(app, h); }
    public void    setSelected(int i, boolean on)    { Native.optionGroupSetSelected(app, h, i, on); }
    public boolean isSelected(int i)                 { return Native.optionGroupIsSelected(app, h, i); }
    public void    onSelectionChanged(IntConsumer c) { Native.onOptionChanged(app, h, (i, t) -> c.accept(i)); }
}
