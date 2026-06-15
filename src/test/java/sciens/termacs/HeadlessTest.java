package sciens.termacs;

/** P3 exit: drives the demo through the Java binding (JNI -> C ABI -> core) on the
 * headless backend and asserts the rendered cells. Run with -ea. */
public class HeadlessTest {

    static boolean rowHas(Application app, String s) {
        for (int y = 0; y < app.screenHeight(); y++)
            if (app.rowText(y).contains(s)) return true;
        return false;
    }

    static void check(boolean cond, String what) {
        if (!cond) { System.err.println("FAIL: " + what); System.exit(1); }
    }

    public static void main(String[] args) {
        Application app = Application.headless(64, 20);
        app.setTheme(Theme.DARK);

        Window win = app.createWindow("termacs — Task Demo");
        win.resize(64, 20);

        MenuBar mb = win.setMenuBar();
        Menu file = mb.addMenu("File");
        Menu help = mb.addMenu("Help");

        final boolean[] quit = {false};
        file.addItem("Quit", () -> quit[0] = true);
        help.addItem("About", () -> app.dialogs().info(win, "termacs demo"));

        VBox body = win.setContentVBox();
        body.setPadding(1);
        body.setSpacing(1);

        HBox row = body.addHBox();
        row.addLabel("Task:");
        LineEdit input = row.addLineEdit();
        input.setSizing(Axis.HORIZONTAL, Sizing.of().preferred(20).stretch(1));
        Button add = row.addButton("Add");

        ListView tasks = body.addListView();
        tasks.setSizing(Axis.VERTICAL, Sizing.of().min(3).stretch(1));

        StatusBar status = win.setStatusBar();

        Runnable addTask = () -> {
            String t = input.text();
            if (t.isEmpty()) return;
            tasks.addItem(t);
            input.setText("");
            status.setText(tasks.count() + " task(s)");
        };
        add.onClicked(addTask);
        input.onSubmitted(addTask);

        status.setText("0 task(s)");
        win.show();
        input.setFocus();

        // type "milk" + Enter
        for (char c : "milk".toCharArray()) app.feedChar(c);
        check(input.text().equals("milk"), "lineedit text == milk");
        app.feed(Key.ENTER);
        check(tasks.count() == 1, "task count == 1");
        check(tasks.itemAt(0).equals("milk"), "item[0] == milk");

        app.render();
        check(rowHas(app, "milk"), "render shows milk");
        check(rowHas(app, "1 task(s)"), "render shows status");
        check(rowHas(app, "File") && rowHas(app, "Help"), "render shows menus");
        check(rowHas(app, "Task:") && rowHas(app, "[ Add ]"), "render shows input row");
        check(rowHas(app, "termacs — Task Demo"), "render shows title");

        // File -> Quit (variant TmEvent menu callback over JNI)
        app.feed(Key.F10);
        app.feed(Key.ENTER);
        check(quit[0], "File>Quit fired across JNI");

        // async confirm dialog
        final boolean[] confirmed = {false};
        app.dialogs().confirm(win, "Remove \"milk\"?", yes -> confirmed[0] = yes);
        app.render();
        check(rowHas(app, "Remove \"milk\"?"), "confirm dialog shown");
        app.feed(Key.ENTER);   // Yes
        check(confirmed[0], "confirm callback got yes");

        app.close();
        System.out.println("Java headless test OK");
    }
}
