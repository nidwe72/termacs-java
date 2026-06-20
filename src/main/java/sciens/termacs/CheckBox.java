package sciens.termacs;

import java.util.function.Consumer;

/** Atomic boolean toggle (§5.10). */
public final class CheckBox extends Widget {
    CheckBox(long app, long h) { super(app, h); }
    public void    setChecked(boolean c) { Native.checkBoxSetChecked(app, h, c); }
    public boolean isChecked()           { return Native.checkBoxIsChecked(app, h); }
    public void    onToggled(Consumer<Boolean> c) { Native.onToggled(app, h, (i, t) -> c.accept(i != 0)); }
}
