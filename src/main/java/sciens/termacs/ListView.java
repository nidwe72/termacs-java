package sciens.termacs;

import java.util.function.IntConsumer;

public final class ListView extends Widget {
    ListView(long app, long h) { super(app, h); }
    public void   addItem(String t)   { Native.listViewAddItem(app, h, t); }
    public void   removeItem(int row) { Native.listViewRemoveItem(app, h, row); }
    public int    count()             { return Native.listViewCount(app, h); }
    public int    selected()          { return Native.listViewSelected(app, h); }
    public String itemAt(int row)     { return Native.listViewItemAt(app, h, row); }
    public void   onActivated(IntConsumer c) { Native.onActivated(app, h, (i, t) -> c.accept(i)); }
}
