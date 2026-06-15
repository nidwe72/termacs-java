package sciens.termacs;

import java.util.function.Consumer;

public final class Dialogs {
    private final Application app;
    Dialogs(Application app) { this.app = app; }

    public void info(Window owner, String message) { Native.dialogInfo(app.ptr(), owner.handle(), message); }
    public void confirm(Window owner, String message, Consumer<Boolean> onResult) {
        Native.dialogConfirm(app.ptr(), owner.handle(), message, (i, t) -> onResult.accept(i == 1));
    }
}
