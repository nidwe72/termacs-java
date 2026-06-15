// termacs — JNI shim. Thin bridge from sciens.termacs.Native to the flat C ABI.
// Statically linked with the C ABI + core + tvision into one libtermacsjni.so.
// Threading: dispatch is single-threaded (§11.6), so callbacks fire on the thread
// that called appRun()/feed — already attached. We GetEnv, attaching only if needed.
#include <jni.h>
#include "termacs/c/termacs.h"
#include <string>

static JavaVM* g_vm = nullptr;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) { g_vm = vm; return JNI_VERSION_1_6; }

namespace {
struct CbRef { jobject obj; };   // global ref to a Native.Cb

JNIEnv* env() {
    JNIEnv* e = nullptr;
    if (g_vm->GetEnv((void**)&e, JNI_VERSION_1_6) != JNI_OK) g_vm->AttachCurrentThread((void**)&e, nullptr);
    return e;
}
void invokeCb(CbRef* r, int i, const char* text) {
    JNIEnv* e = env();
    jclass cls = e->GetObjectClass(r->obj);
    jmethodID m = e->GetMethodID(cls, "invoke", "(ILjava/lang/String;)V");
    jstring s = text ? e->NewStringUTF(text) : nullptr;
    e->CallVoidMethod(r->obj, m, (jint)i, s);
    if (s) e->DeleteLocalRef(s);
    e->DeleteLocalRef(cls);
}
void slotTramp(const TmEvent* ev, void* user)  { invokeCb((CbRef*)user, ev ? ev->i : 0, ev ? ev->text : nullptr); }
void thunkTramp(void* user)                     { invokeCb((CbRef*)user, 0, nullptr); }
CbRef* mkRef(JNIEnv* e, jobject cb)             { auto* r = new CbRef(); r->obj = e->NewGlobalRef(cb); return r; }
std::string S(JNIEnv* e, jstring s) {
    if (!s) return {};
    const char* c = e->GetStringUTFChars(s, nullptr);
    std::string r = c ? c : "";
    e->ReleaseStringUTFChars(s, c);
    return r;
}
} // namespace

#define APP ((tm_app*)app)
#define W   ((tm_widget)w)

extern "C" {

#define JFN(ret, name) JNIEXPORT ret JNICALL Java_sciens_termacs_Native_##name

JFN(jint, abiVersion)(JNIEnv*, jclass)            { return tm_abi_version(); }
JFN(jint, lastStatus)(JNIEnv*, jclass)            { return tm_last_status(); }
JFN(jstring, statusMessage)(JNIEnv* e, jclass)    { return e->NewStringUTF(tm_status_message()); }

JFN(jlong, appNewHeadless)(JNIEnv*, jclass, jint w, jint h) { return (jlong)tm_app_new_headless(w, h); }
JFN(jlong, appNewTerminal)(JNIEnv*, jclass)       { return (jlong)tm_app_new_terminal(); }
JFN(void, appFree)(JNIEnv*, jclass, jlong app)    { tm_app_free(APP); }
JFN(void, appSetTheme)(JNIEnv*, jclass, jlong app, jint t) { tm_app_set_theme(APP, (TmTheme)t); }
JFN(jint, appRun)(JNIEnv*, jclass, jlong app)     { return tm_app_run(APP); }
JFN(void, appQuit)(JNIEnv*, jclass, jlong app, jint c) { tm_app_quit(APP, c); }
JFN(void, appPost)(JNIEnv* e, jclass, jlong app, jobject cb) { tm_app_post(APP, thunkTramp, mkRef(e, cb)); }
JFN(jlong, appStartTimer)(JNIEnv* e, jclass, jlong app, jint ms, jobject cb) { return (jlong)tm_app_start_timer(APP, ms, thunkTramp, mkRef(e, cb)); }
JFN(void, appStopTimer)(JNIEnv*, jclass, jlong app, jlong id) { tm_app_stop_timer(APP, (uint64_t)id); }
JFN(void, appRender)(JNIEnv*, jclass, jlong app)  { tm_app_render(APP); }
JFN(jint, appScreenW)(JNIEnv*, jclass, jlong app) { int w = 0, h = 0; tm_app_screen_size(APP, &w, &h); return w; }
JFN(jint, appScreenH)(JNIEnv*, jclass, jlong app) { int w = 0, h = 0; tm_app_screen_size(APP, &w, &h); return h; }
JFN(jstring, appRowText)(JNIEnv* e, jclass, jlong app, jint y) { char b[1024]; tm_app_row_text(APP, y, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, appFeedKey)(JNIEnv*, jclass, jlong app, jint k, jint ch) { tm_app_feed_key(APP, (TmKey)k, (uint32_t)ch); }

JFN(jlong, createWindow)(JNIEnv* e, jclass, jlong app, jstring t) { auto s = S(e, t); return (jlong)tm_create_window(APP, s.c_str()); }
JFN(void, windowResize)(JNIEnv*, jclass, jlong app, jlong w, jint ww, jint hh) { tm_window_resize(APP, W, ww, hh); }
JFN(jlong, windowSetMenubar)(JNIEnv*, jclass, jlong app, jlong w)     { return (jlong)tm_window_set_menubar(APP, W); }
JFN(jlong, windowSetStatusbar)(JNIEnv*, jclass, jlong app, jlong w)   { return (jlong)tm_window_set_statusbar(APP, W); }
JFN(jlong, windowSetContentVbox)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_window_set_content_vbox(APP, W); }
JFN(jlong, windowSetContentHbox)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_window_set_content_hbox(APP, W); }
JFN(void, windowShow)(JNIEnv*, jclass, jlong app, jlong w)           { tm_window_show(APP, W); }

JFN(void, containerSetPadding)(JNIEnv*, jclass, jlong app, jlong w, jint c) { tm_container_set_padding(APP, W, c); }
JFN(void, containerSetSpacing)(JNIEnv*, jclass, jlong app, jlong w, jint c) { tm_container_set_spacing(APP, W, c); }
JFN(jlong, addLabel)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)  { auto s = S(e, t); return (jlong)tm_add_label(APP, W, s.c_str()); }
JFN(jlong, addButton)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); return (jlong)tm_add_button(APP, W, s.c_str()); }
JFN(jlong, addLineEdit)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_add_lineedit(APP, W); }
JFN(jlong, addListView)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_add_listview(APP, W); }
JFN(jlong, addHBox)(JNIEnv*, jclass, jlong app, jlong w)     { return (jlong)tm_add_hbox(APP, W); }
JFN(jlong, addVBox)(JNIEnv*, jclass, jlong app, jlong w)     { return (jlong)tm_add_vbox(APP, W); }

JFN(void, widgetSetSizing)(JNIEnv*, jclass, jlong app, jlong w, jint ax, jint mn, jint pf, jint mx, jint st) { tm_widget_set_sizing(APP, W, (TmAxis)ax, mn, pf, mx, st); }
JFN(void, widgetSetFocus)(JNIEnv*, jclass, jlong app, jlong w) { tm_widget_set_focus(APP, W); }
JFN(void, widgetRemove)(JNIEnv*, jclass, jlong app, jlong w)   { tm_widget_remove(APP, W); }

JFN(void, labelSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)    { auto s = S(e, t); tm_label_set_text(APP, W, s.c_str()); }
JFN(void, buttonSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)   { auto s = S(e, t); tm_button_set_text(APP, W, s.c_str()); }
JFN(void, lineEditSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_lineedit_set_text(APP, W, s.c_str()); }
JFN(jstring, lineEditText)(JNIEnv* e, jclass, jlong app, jlong w) { char b[1024]; tm_lineedit_text(APP, W, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, listViewAddItem)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_listview_add_item(APP, W, s.c_str()); }
JFN(void, listViewRemoveItem)(JNIEnv*, jclass, jlong app, jlong w, jint row) { tm_listview_remove_item(APP, W, row); }
JFN(jint, listViewCount)(JNIEnv*, jclass, jlong app, jlong w)    { return tm_listview_count(APP, W); }
JFN(jint, listViewSelected)(JNIEnv*, jclass, jlong app, jlong w) { return tm_listview_selected(APP, W); }
JFN(jstring, listViewItemAt)(JNIEnv* e, jclass, jlong app, jlong w, jint row) { char b[1024]; tm_listview_item_at(APP, W, row, b, sizeof b); return e->NewStringUTF(b); }

JFN(jlong, menubarAddMenu)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); return (jlong)tm_menubar_add_menu(APP, W, s.c_str()); }
JFN(void, menuAddItem)(JNIEnv* e, jclass, jlong app, jlong w, jstring t, jobject cb) { auto s = S(e, t); tm_menu_add_item(APP, W, s.c_str(), slotTramp, mkRef(e, cb)); }
JFN(void, statusbarSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)      { auto s = S(e, t); tm_statusbar_set_text(APP, W, s.c_str()); }
JFN(void, statusbarSetRightText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_statusbar_set_right_text(APP, W, s.c_str()); }

JFN(void, onClicked)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)     { tm_on_clicked(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, onSubmitted)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)   { tm_on_submitted(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, onTextChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_text_changed(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, onActivated)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)   { tm_on_activated(APP, W, slotTramp, mkRef(e, cb)); }

JFN(void, dialogInfo)(JNIEnv* e, jclass, jlong app, jlong w, jstring m)              { auto s = S(e, m); tm_dialog_info(APP, W, s.c_str()); }
JFN(void, dialogConfirm)(JNIEnv* e, jclass, jlong app, jlong w, jstring m, jobject cb) { auto s = S(e, m); tm_dialog_confirm(APP, W, s.c_str(), slotTramp, mkRef(e, cb)); }

} // extern "C"
